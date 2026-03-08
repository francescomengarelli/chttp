PROJECT     := chttp
CC          := clang
AR          := ar
ARFLAGS     := rcs

CFLAGS      := -Wall -Wextra -Werror -Iinclude -fPIC -MMD -MP

# Directories
SRC_DIR     := src
INC_DIR     := include
TESTS_DIR   := tests
EXAMPLES_DIR:= examples
BUILD_DIR   := build
OBJ_DIR     := $(BUILD_DIR)/obj
LIB_DIR     := $(BUILD_DIR)/lib
BIN_DIR     := $(BUILD_DIR)/bin

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    SHARED_EXT := dylib
    SHARED_FLAGS := -dynamiclib
else
    SHARED_EXT := so
    SHARED_FLAGS := -shared
endif

# Files
SRCS        := $(wildcard $(SRC_DIR)/*.c)
OBJS        := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DEPS        := $(OBJS:.o=.d)

STATIC_LIB  := $(LIB_DIR)/lib$(PROJECT).a
SHARED_LIB  := $(LIB_DIR)/lib$(PROJECT).$(SHARED_EXT)

TEST_SRCS   := $(wildcard $(TESTS_DIR)/*.c)
TEST_BINS   := $(patsubst $(TESTS_DIR)/%.c, $(BIN_DIR)/test_%, $(TEST_SRCS))

EXAMPLE_SRCS := $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_BINS := $(patsubst $(EXAMPLES_DIR)/%.c, $(BIN_DIR)/ex_%, $(EXAMPLE_SRCS))

.PHONY: all clean test examples

all: $(STATIC_LIB) $(SHARED_LIB)

$(STATIC_LIB): $(OBJS)
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) $@ $^

$(SHARED_LIB): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(SHARED_FLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@


test: $(TEST_BINS)
	@for t in $(TEST_BINS); do ./$$t || exit 1; done

$(BIN_DIR)/test_%: $(TESTS_DIR)/%.c $(STATIC_LIB)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@ $(STATIC_LIB)


examples: $(EXAMPLE_BINS)

$(BIN_DIR)/ex_%: $(EXAMPLES_DIR)/%.c $(STATIC_LIB)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@ $(STATIC_LIB)

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)
