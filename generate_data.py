#!/usr/bin/env python3
"""
Binary file generator for Item records defined in src/Item.hpp.

Item memory layout (x86_64 Linux ABI):
    - int key: 4 bytes (offset 0)
    - padding: 4 bytes (offset 4)
    - long int value: 8 bytes (offset 8)
    - std::array<char, 5000> text: 5000 bytes (offset 16)
    Total size per Item: 5016 bytes.

Workflow:
    1. Generates the ascending file first in the tmp/ directory.
    2. Generates the descending and shuffled files based directly on the ascending file:
       - Descending: reads ascending file items and reverses them.
       - Shuffled: reads ascending file items and swaps two random positions
         at least 1,000,000 times.
    3. Always produces all 3 files inside the project's tmp/ directory.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import os
from pathlib import Path
import random
import struct
import sys
import time


ITEM_STRUCT_FORMAT = "@i l 5000s"
TEXT_SIZE = 5000
DEFAULT_QUANTITY = 2_000_000
DEFAULT_MIN_SWAPS = 1_000_000


@dataclass(slots=True)
class Item:
    key: int
    value: int
    text: bytes


class BinaryFileGenerator:
    """
    Generates binary files containing Item records according to src/Item.hpp.
    """

    def __init__(
        self,
        quantity: int = DEFAULT_QUANTITY,
        start_key: int = 1,
        text_size: int = TEXT_SIZE,
        default_payload: bytes | None = None,
        random_values: bool = True,
    ) -> None:
        self.quantity = quantity
        self.start_key = start_key
        self.text_size = text_size
        self.random_values = random_values
        self.struct_format = f"@i l {self.text_size}s"
        self.item_struct = struct.Struct(self.struct_format)
        self.item_size = self.item_struct.size

        if default_payload is None:
            self.default_payload = b"\x00" * self.text_size
        else:
            self.default_payload = default_payload[: self.text_size].ljust(
                self.text_size, b"\x00"
            )

        self.items: list[Item] = []

    def generate_ascending(self) -> None:
        """
        Populates items in strictly ascending order (start_key to start_key + quantity - 1).
        """
        print(f"\n[1/3] Creating {self.quantity:,} items in ASCENDING order...")
        t0 = time.perf_counter()

        keys = range(self.start_key, self.start_key + self.quantity)
        payload = self.default_payload

        if self.random_values:
            self.items = [
                Item(key=k, value=random.randint(0, 2**62 - 1), text=payload)
                for k in keys
            ]
        else:
            self.items = [
                Item(key=k, value=k * 10, text=payload)
                for k in keys
            ]

        dt_gen = time.perf_counter() - t0
        print(f"Created {len(self.items):,} ascending items in {dt_gen:.2f}s.")

    @classmethod
    def load_from_file(
        cls,
        filepath: str | Path,
        text_size: int = TEXT_SIZE,
        chunk_size: int = 10_000,
    ) -> BinaryFileGenerator:
        """
        Loads items directly from an existing binary file into a BinaryFileGenerator instance.
        """
        filepath = Path(filepath)
        item_struct = struct.Struct(f"@i l {text_size}s")
        item_size = item_struct.size
        file_size = filepath.stat().st_size

        if file_size % item_size != 0:
            raise ValueError(
                f"File size {file_size} is not a multiple of Item size {item_size}"
            )

        total_items = file_size // item_size
        gen = cls(quantity=total_items, text_size=text_size)
        items: list[Item] = []

        buffer = bytearray(chunk_size * item_size)
        shared_text: bytes | None = None

        t0 = time.perf_counter()
        with open(filepath, "rb") as f:
            while True:
                nbytes = f.readinto(buffer)
                if not nbytes:
                    break
                chunk_items = nbytes // item_size
                for idx in range(chunk_items):
                    k, v, raw_txt = item_struct.unpack_from(buffer, idx * item_size)
                    if shared_text is None:
                        shared_text = bytes(raw_txt)
                        txt_to_use = shared_text
                    elif raw_txt == shared_text:
                        txt_to_use = shared_text
                    else:
                        txt_to_use = bytes(raw_txt)
                    items.append(Item(key=k, value=v, text=txt_to_use))

        dt = time.perf_counter() - t0
        print(f"Loaded {len(items):,} items from {filepath.name} in {dt:.2f}s.")
        gen.items = items
        return gen

    def make_descending(self) -> None:
        """
        Reverses the existing items sequence in-place.
        """
        print(f"\n[2/3] Reversing ascending items to create DESCENDING order...")
        t0 = time.perf_counter()
        self.items.reverse()
        dt = time.perf_counter() - t0
        print(f"Reversed {len(self.items):,} items in {dt:.2f}s.")

    def swap(self, pos1: int | None = None, pos2: int | None = None) -> tuple[int, int]:
        """
        Swap method: gets two random positions and swaps them
        (or swaps pos1 and pos2 if explicitly specified).
        Returns the two swapped indices (pos1, pos2).
        """
        n = len(self.items)
        if pos1 is None:
            pos1 = random.randint(0, n - 1)
        if pos2 is None:
            pos2 = random.randint(0, n - 1)

        self.items[pos1], self.items[pos2] = self.items[pos2], self.items[pos1]
        return pos1, pos2

    def shuffle(self, min_swaps: int = DEFAULT_MIN_SWAPS, report_interval: int = 250_000) -> None:
        """
        Shuffled algorithm implemented with the swap method:
        Gets two random positions and swaps them at least min_swaps times.
        """
        n = len(self.items)
        if n < 2:
            return

        print(f"\n[3/3] Shuffling {n:,} items based on ascending file using swap method ({min_swaps:,} swaps)...")
        t0 = time.perf_counter()

        for count in range(1, min_swaps + 1):
            self.swap()
            if report_interval > 0 and (count % report_interval == 0 or count == min_swaps):
                elapsed = time.perf_counter() - t0
                rate = count / elapsed if elapsed > 0 else 0
                pct = (count / min_swaps) * 100
                print(f"  -> {count:,}/{min_swaps:,} swaps ({pct:.1f}%) [{rate:,.0f} swaps/sec]")

        dt_shuffle = time.perf_counter() - t0
        print(f"Shuffle complete: {min_swaps:,} swaps performed in {dt_shuffle:.2f}s.")

    def save_to_file(self, output_path: str | Path, chunk_size: int = 10_000) -> Path:
        """
        Writes all items into a binary file in optimized chunks.
        """
        output_path = Path(output_path)
        output_path.parent.mkdir(parents=True, exist_ok=True)

        total_items = len(self.items)
        item_struct = self.item_struct
        item_size = self.item_size
        total_bytes = total_items * item_size

        print(
            f"Writing {total_items:,} items "
            f"({total_bytes / (1024**3):.2f} GiB / {total_bytes:,} bytes) to {output_path}..."
        )

        chunk_buf = bytearray(chunk_size * item_size)
        t0 = time.perf_counter()

        with open(output_path, "wb") as f:
            for offset in range(0, total_items, chunk_size):
                batch = self.items[offset : offset + chunk_size]
                batch_count = len(batch)
                for idx, it in enumerate(batch):
                    item_struct.pack_into(
                        chunk_buf,
                        idx * item_size,
                        it.key,
                        it.value,
                        it.text,
                    )
                f.write(memoryview(chunk_buf)[: batch_count * item_size])

        dt_write = time.perf_counter() - t0
        mb_per_sec = (total_bytes / (1024 * 1024)) / dt_write if dt_write > 0 else 0
        print(f"Finished writing {output_path.name} in {dt_write:.2f}s ({mb_per_sec:.1f} MB/s).")
        return output_path


def create_symlink(target_path: Path, link_path: Path) -> None:
    """
    Creates or updates a symlink pointing to target_path.
    """
    try:
        if link_path.is_symlink() or link_path.exists():
            link_path.unlink()
        link_path.symlink_to(target_path.name)
    except OSError:
        pass


def verify_file(filepath: str | Path, preview_count: int = 4) -> None:
    """
    Reads and prints a preview of the items from the binary file.
    """
    filepath = Path(filepath)
    if not filepath.exists():
        print(f"File not found: {filepath}", file=sys.stderr)
        return

    item_struct = struct.Struct(ITEM_STRUCT_FORMAT)
    item_size = item_struct.size
    file_size = filepath.stat().st_size

    if file_size % item_size != 0:
        print(
            f"WARNING: File size {file_size} is not a multiple of Item size {item_size}!",
            file=sys.stderr,
        )
        return

    num_items = file_size // item_size
    print(f"File: {filepath}")
    print(f"  Total size: {file_size:,} bytes ({file_size / (1024**3):.2f} GiB) | Entries: {num_items:,}")

    with open(filepath, "rb") as f:
        show_head = min(preview_count, num_items)
        print("  First entries:")
        for i in range(show_head):
            data = f.read(item_size)
            key, val, raw_txt = item_struct.unpack(data)
            txt_preview = raw_txt.split(b"\x00")[0].decode(errors="replace")
            print(f"    [{i:>{len(str(num_items))}}] key={key:<10} value={val:<20} text='{txt_preview}'")

        if num_items > preview_count * 2:
            print("    ...")
            tail_count = min(preview_count, num_items - show_head)
            start_tail_idx = num_items - tail_count
            f.seek(start_tail_idx * item_size)
            print("  Last entries:")
            for i in range(start_tail_idx, num_items):
                data = f.read(item_size)
                key, val, raw_txt = item_struct.unpack(data)
                txt_preview = raw_txt.split(b"\x00")[0].decode(errors="replace")
                print(f"    [{i:>{len(str(num_items))}}] key={key:<10} value={val:<20} text='{txt_preview}'")
    print()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Generate binary files of Item records (src/Item.hpp) in ascending, descending, "
            "and shuffled order inside project's tmp/ directory. "
            "Ascending file is generated first, and the other two are generated based on it."
        ),
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "-n",
        "--quantity",
        type=int,
        default=DEFAULT_QUANTITY,
        help="Number of Item entries to generate for each file.",
    )
    parser.add_argument(
        "-s",
        "--swaps",
        type=int,
        default=DEFAULT_MIN_SWAPS,
        help="Number of swaps for the shuffled algorithm (must be at least 1,000,000 for full runs).",
    )
    parser.add_argument(
        "-d",
        "--output-dir",
        type=str,
        default="tmp",
        help="Directory to save the generated binary files.",
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=None,
        help="Optional random seed for reproducible results.",
    )
    parser.add_argument(
        "--start-key",
        type=int,
        default=1,
        help="Starting key integer value.",
    )
    parser.add_argument(
        "--text-size",
        type=int,
        default=TEXT_SIZE,
        help="Size of text payload array in bytes.",
    )
    parser.add_argument(
        "--no-random-values",
        action="store_true",
        help="Use deterministic values (key * 10) instead of random long ints.",
    )
    parser.add_argument(
        "--no-verify",
        action="store_true",
        help="Skip verification preview after file generation.",
    )
    parser.add_argument(
        "--preview-only",
        type=str,
        default=None,
        help="Only verify and preview an existing binary file without generating anything.",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()

    if args.preview_only:
        verify_file(args.preview_only)
        return

    if args.seed is not None:
        random.seed(args.seed)

    # Validate swaps requirement
    if args.quantity >= DEFAULT_QUANTITY and args.swaps < DEFAULT_MIN_SWAPS:
        print(
            f"WARNING: The specification requires at least 1,000,000 swaps for shuffled data. "
            f"Provided: {args.swaps:,}. Setting swaps to {DEFAULT_MIN_SWAPS:,}.",
            file=sys.stderr,
        )
        args.swaps = DEFAULT_MIN_SWAPS

    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    asc_file = output_dir / f"items_ascending_{args.quantity}.bin"
    desc_file = output_dir / f"items_descending_{args.quantity}.bin"
    shuf_file = output_dir / f"items_shuffled_{args.quantity}.bin"

    print("=================================================================")
    print(f"Generating 3 binary files in directory: {output_dir.resolve()}")
    print(f"Entries per file: {args.quantity:,} (Item size: {struct.calcsize(ITEM_STRUCT_FORMAT)} bytes)")
    print(f"Total size per file: {args.quantity * struct.calcsize(ITEM_STRUCT_FORMAT) / (1024**3):.2f} GiB")
    print(f"Swaps for shuffled file: {args.swaps:,}")
    print("=================================================================")

    # -------------------------------------------------------------
    # Step 1: Generate the ASCENDING file first
    # -------------------------------------------------------------
    t_start_all = time.perf_counter()
    asc_gen = BinaryFileGenerator(
        quantity=args.quantity,
        start_key=args.start_key,
        text_size=args.text_size,
        random_values=not args.no_random_values,
    )
    asc_gen.generate_ascending()
    asc_gen.save_to_file(asc_file)
    create_symlink(asc_file, output_dir / "items_ascending.bin")

    # -------------------------------------------------------------
    # Step 2: Generate DESCENDING file based on the ascending file
    # -------------------------------------------------------------
    desc_gen = BinaryFileGenerator.load_from_file(asc_file, text_size=args.text_size)
    desc_gen.make_descending()
    desc_gen.save_to_file(desc_file)
    create_symlink(desc_file, output_dir / "items_descending.bin")

    # -------------------------------------------------------------
    # Step 3: Generate SHUFFLED file based on the ascending file
    # -------------------------------------------------------------
    shuf_gen = BinaryFileGenerator.load_from_file(asc_file, text_size=args.text_size)
    shuf_gen.shuffle(min_swaps=args.swaps)
    shuf_gen.save_to_file(shuf_file)
    create_symlink(shuf_file, output_dir / "items_shuffled.bin")

    total_time = time.perf_counter() - t_start_all
    print("\n=================================================================")
    print(f"All 3 files generated successfully in {total_time:.2f}s!")
    print("=================================================================\n")

    if not args.no_verify:
        print("--- Verifying Generated Files ---")
        verify_file(asc_file)
        verify_file(desc_file)
        verify_file(shuf_file)


if __name__ == "__main__":
    main()
