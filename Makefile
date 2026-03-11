.DEFAULT_GOAL := all

PROJECT     := chttp
DEBUG ?= 0
SHARED ?= 0
CONFIG := release

CC          := gcc
CFLAGS      := -Wall -Wextra -Werror -Iinclude -Isrc -fPIC -MMD -MP

AR          := ar
ARFLAGS     := rcs

SRC_DIR     := src
INC_DIR     := include
TESTS_DIR   := tests
EXAMPLES_DIR := examples
BUILD_DIR   := build
OBJ_DIR     = $(BUILD_DIR)/obj/$(CONFIG)
LIB_DIR     = $(BUILD_DIR)/lib/$(CONFIG)
BIN_DIR     = $(BUILD_DIR)/bin/$(CONFIG)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    SHARED_EXT := dylib
    SHARED_FLAGS := -dynamiclib
else
    SHARED_EXT := so
    SHARED_FLAGS := -shared
endif

ifeq ($(DEBUG),1)
    CFLAGS += -g -O0 -DDEBUG
    CONFIG := debug
else
    CFLAGS += -O3 -DNDEBUG
endif

# Files
SRCS_COMMON     = src/internal/httpssn.c src/internal/dynarr.c src/internal/io.c src/internal/uri.c src/error.c src/internal/utils.c
SRCS_SERVER     = src/server.c
SRCS_CLIENT     = src/client.c

TEST_SRCS       = $(wildcard $(TESTS_DIR)/*.c)
TEST_BINS       = $(patsubst $(TESTS_DIR)/%.c, $(BIN_DIR)/test_%, $(TEST_SRCS))

EXAMPLE_SRCS    = $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_BINS    = $(patsubst $(EXAMPLES_DIR)/%.c, $(BIN_DIR)/ex_%, $(EXAMPLE_SRCS))



ALL_STATIC_LIBS :=
ALL_SHARED_LIBS :=
DEPS            :=

define LIB_TEMPLATE
OBJS$(1) := $$(patsubst $(SRC_DIR)/%.c, $$(OBJ_DIR)$(1)/%.o, $(2))
DEPS += $$(OBJS$(1):.o=.d)
STATIC_LIB$(1) := $$(LIB_DIR)/lib$(PROJECT)$(1).a
SHARED_LIB$(1) := $$(LIB_DIR)/lib$(PROJECT)$(1).$$(SHARED_EXT)
ALL_STATIC_LIBS += $$(STATIC_LIB$(1))
ALL_SHARED_LIBS += $$(SHARED_LIB$(1))
$$(OBJ_DIR)$(1)/%.o: CFLAGS += $(3)
$$(OBJ_DIR)$(1)/%.o: $$(SRC_DIR)/%.c
	@mkdir -p $$(@D)
	$$(CC) $$(CFLAGS) -c $$< -o $$@
$$(STATIC_LIB$(1)): $$(OBJS$(1))
	@mkdir -p $$(@D)
	$$(AR) $$(ARFLAGS) $$@ $$^
$$(SHARED_LIB$(1)): $$(OBJS$(1))
	@mkdir -p $$(@D)
	$$(CC) $$(SHARED_FLAGS) $$^ -o $$@
endef

$(eval $(call LIB_TEMPLATE,_server, $(SRCS_COMMON) $(SRCS_SERVER), -DCHTTP_SERVER))

$(eval $(call LIB_TEMPLATE,_client, $(SRCS_COMMON) $(SRCS_CLIENT), -DCHTTP_CLIENT))

$(eval $(call LIB_TEMPLATE,, $(SRCS_COMMON) $(SRCS_SERVER) $(SRCS_CLIENT),))

.PHONY: all clean test examples server client

all: $(STATIC_LIB) $(SHARED_LIB)

server: $(STATIC_LIB_server) $(SHARED_LIB_server)

client: $(STATIC_LIB_client) $(SHARED_LIB_client)

test: $(TEST_BINS)
	@for t in $(TEST_BINS); do echo "RUNNING $$t..."; ./$$t && echo "PASSED" || exit 1; done

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
