{ pkgs, lib, config, inputs, ... }:

{
  # https://devenv.sh/packages/
  packages = [
    pkgs.git
    pkgs.clang
    pkgs.clang-tools
  ];

  # https://devenv.sh/languages/
  languages.cplusplus = {
    enable = true;
    lsp.package = pkgs.clang;
  };

  languages.python.enable = true;

  # Set compiler environment variables to Clang
  env.CXX = "clang++";
  env.CC = "clang";

  # https://devenv.sh/git-hooks/
  git-hooks.hooks = {
    clang-format.enable = true;
    clang-tidy.enable = true;
  };

  # https://devenv.sh/scripts/
  scripts = {
    generate = {
      description = "Generate binary files of Items (ascending, descending, shuffled)";
      exec = ''
        python3 generate_data.py "$@"
      '';
    };

    build = {
      description = "Build the C++ project and generate compile_commands.json";
      exec = ''
        set -e
        SRC_DIR="''${SRC_DIR:-src}"
        BUILD_DIR="''${BUILD_DIR:-build}"
        TARGET="''${TARGET:-$BUILD_DIR/pesquisa}"
        CXX="clang++"
        CXXFLAGS="''${CXXFLAGS:--std=c++23 -Wall -Wextra -Wpedantic -O0 -g}"
        CPPFLAGS="''${CPPFLAGS:--Iinclude}"
        LDFLAGS="''${LDFLAGS:-}"
        LDLIBS="''${LDLIBS:-}"

        mkdir -p "$BUILD_DIR"
        mapfile -t CPP_FILES < <(find "$SRC_DIR" -type f -name '*.cpp')

        if [ ''${#CPP_FILES[@]} -eq 0 ]; then
          echo "error: no C++ source files found in $SRC_DIR"
          exit 1
        fi

        OBJECTS=()
        MJ_FILES=()
        for file in "''${CPP_FILES[@]}"; do
          rel_path="''${file#$SRC_DIR/}"
          obj_dir="$BUILD_DIR/$(dirname "$rel_path")"
          mkdir -p "$obj_dir"
          obj="$BUILD_DIR/''${rel_path%.cpp}.o"
          mj="$BUILD_DIR/''${rel_path%.cpp}.json"
          OBJECTS+=("$obj")
          MJ_FILES+=("$mj")
          echo "compile $file"
          $CXX $CXXFLAGS $CPPFLAGS -MJ "$mj" -c "$file" -o "$obj"
        done

        echo "link    $TARGET"
        $CXX "''${OBJECTS[@]}" $LDFLAGS $LDLIBS -o "$TARGET"

        echo "[" > "$BUILD_DIR/compile_commands.json"
        cat "''${MJ_FILES[@]}" | sed '$ s/,$//' >> "$BUILD_DIR/compile_commands.json"
        echo "]" >> "$BUILD_DIR/compile_commands.json"
        rm -f "''${MJ_FILES[@]}"
      '';
    };

    format = {
      description = "Format C++ source and header files using clang-format";
      exec = ''
        set -e
        CLANG_FORMAT="''${CLANG_FORMAT:-clang-format}"
        SRC_DIR="''${SRC_DIR:-src}"

        command -v "$CLANG_FORMAT" >/dev/null || {
          echo "error: $CLANG_FORMAT was not found in PATH"
          exit 1
        }

        SRC_DIRS=()
        [ -d "$SRC_DIR" ] && SRC_DIRS+=("$SRC_DIR")
        [ -d include ] && SRC_DIRS+=(include)

        if [ ''${#SRC_DIRS[@]} -eq 0 ]; then
          echo "error: neither $SRC_DIR nor include directory found"
          exit 1
        fi

        mapfile -t CPP_FILES < <(find "''${SRC_DIRS[@]}" -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \))

        if [ ''${#CPP_FILES[@]} -eq 0 ]; then
          echo "No C++ files to format."
          exit 0
        fi

        for file in "''${CPP_FILES[@]}"; do
          echo "format  $file"
          "$CLANG_FORMAT" -i -style=file "$file"
        done
      '';
    };

    "format-check" = {
      description = "Check formatting of C++ source and header files";
      exec = ''
        set -e
        CLANG_FORMAT="''${CLANG_FORMAT:-clang-format}"
        SRC_DIR="''${SRC_DIR:-src}"

        command -v "$CLANG_FORMAT" >/dev/null || {
          echo "error: $CLANG_FORMAT was not found in PATH"
          exit 1
        }

        SRC_DIRS=()
        [ -d "$SRC_DIR" ] && SRC_DIRS+=("$SRC_DIR")
        [ -d include ] && SRC_DIRS+=(include)

        if [ ''${#SRC_DIRS[@]} -eq 0 ]; then
          echo "error: neither $SRC_DIR nor include directory found"
          exit 1
        fi

        mapfile -t CPP_FILES < <(find "''${SRC_DIRS[@]}" -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \))

        if [ ''${#CPP_FILES[@]} -eq 0 ]; then
          exit 0
        fi

        status=0
        for file in "''${CPP_FILES[@]}"; do
          if ! "$CLANG_FORMAT" --dry-run --Werror -style=file "$file"; then
            status=1
          fi
        done
        exit $status
      '';
    };

    tidy = {
      description = "Run clang-tidy on C++ source files";
      exec = ''
        set -e
        CLANG_TIDY="''${CLANG_TIDY:-clang-tidy}"
        SRC_DIR="''${SRC_DIR:-src}"
        BUILD_DIR="''${BUILD_DIR:-build}"

        command -v "$CLANG_TIDY" >/dev/null || {
          echo "error: $CLANG_TIDY was not found in PATH"
          exit 1
        }

        if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
          echo "error: $BUILD_DIR/compile_commands.json not found"
          echo "hint: run 'build' first to generate compilation database"
          exit 1
        fi

        mapfile -t CPP_FILES < <(find "$SRC_DIR" -type f -name '*.cpp')

        if [ ''${#CPP_FILES[@]} -eq 0 ]; then
          echo "No C++ files found to tidy."
          exit 0
        fi

        for file in "''${CPP_FILES[@]}"; do
          echo "tidy    $file"
          "$CLANG_TIDY" -p "$BUILD_DIR" "$file" "$@"
        done
      '';
    };

    lint = {
      description = "Run linter (alias for tidy)";
      exec = ''
        tidy "$@"
      '';
    };

    clean = {
      description = "Remove build artifacts";
      exec = ''
        BUILD_DIR="''${BUILD_DIR:-build}"
        rm -rf "$BUILD_DIR"
      '';
    };
  };

  # See full reference at https://devenv.sh/reference/options/
}
