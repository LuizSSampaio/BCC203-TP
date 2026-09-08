CXX := clang
CXXFLAGS  := -std=c++23 -Wall -Wextra -Wpedantic -O0 -g
CXX_FORMAT := clang-format
CXX_LINTER := clang-tidy
CPPFLAGS  := -Iinclude
LDFLAGS   :=
LDLIBS    :=

SRC_DIR   := src
BUILD_DIR := build
TARGET := $(BUILD_DIR)/pesquisa


CPP_FILES := $(shell find $(SRC_DIR) \
	-prune -o \
	-type f -name '*.cpp' -print)

build:
	mkdir -p $(BUILD_DIR)
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $(CPP_FILES) -o $(TARGET)

format:
	@command -v "$(CLANG_FORMAT)" >/dev/null || { \
		echo "error: $(CLANG_FORMAT) was not found in PATH"; exit 1; \
	}
	@set -e; \
	for file in $(CPP_FILES); do \
		echo "format  $$file"; \
		"$(CLANG_FORMAT)" -i -style=file "$$file"; \
	done

format-check:
	@command -v "$(CLANG_FORMAT)" >/dev/null || { \
		echo "error: $(CLANG_FORMAT) was not found in PATH"; exit 1; \
	}
	@set -e; \
	status=0; \
	for file in $(CPP_FILES); do \
		if ! "$(CLANG_FORMAT)" --dry-run --Werror -style=file "$$file"; then \
			status=1; \
		fi; \
	done; \
	exit $$status

tidy:
	@command -v "$(CLANG_TIDY)" >/dev/null || { \
		echo "error: $(CLANG_TIDY) was not found in PATH"; exit 1; \
	}
	@test -f "$(BUILD_DIR)/compile_commands.json" || { \
		echo "error: $(BUILD_DIR)/compile_commands.json not found"; \
		echo "hint: configure with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"; \
		exit 1; \
	}
	@set -e; \
	for file in $(CPP_FILES); do \
		echo "tidy    $$file"; \
		"$(CLANG_TIDY)" -p "$(BUILD_DIR)" "$$file"; \
	done

lint: tidy

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
