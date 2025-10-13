export CC = gcc
C_VERSION = 89

LIBS = curl

INCLUDE_DIRS = include src
INCLUDE_FILES = include/score/types/types.h
CFLAGS = -std=c$(C_VERSION) -Wall -Wextra -Werror -Wpedantic $(addprefix -include ,$(INCLUDE_FILES)) $(addprefix -I,$(INCLUDE_DIRS)) $(addprefix -l,$(LIBS))

SRC_DIR := src
SRC_FILES := $(shell find $(SRC_DIR) -name "*.c")

BUILD_DIR := $(CURDIR)/build
export OBJ_DIR := $(BUILD_DIR)/obj

export SCORE_DEFINES = SCORE_USE_STDINT SCORE_UTILS SCORE_TYPES SCORE_ENABLE_ASSERTS SCORE_CONSOLE SCORE_HTTP SCORE_LINKED_LIST SCORE_STRING SCORE_BUFFER SCORE_JSON SCORE_JSON_CJSON SCORE_TIME SCORE_TIME_NOW SCORE_TIME_NOW_LINUX SCORE_INTEGRATIONS_OPEN_METEO SCORE_INTEGRATIONS_LOCATIONIQ
DEFINES = $(SCORE_DEFINES)
DEFINES_PREFIXED = $(addprefix -D,$(DEFINES))

PROGRAM_OBJ_DIR = $(BUILD_DIR)/program_obj
BIN_DIR := $(BUILD_DIR)/bin
BIN := $(BIN_DIR)/program

PROGRAM_OBJS = $(patsubst $(SRC_DIR)/%.c,$(PROGRAM_OBJ_DIR)/%.o,$(SRC_FILES))

all: build_score $(PROGRAM_OBJS) $(BIN)
	@echo "Build done."

build_score:
	@$(MAKE) -C include/score

build_program:
	@echo "Building program ..."
	@echo "Defines:" $(DEFINES)
	@echo "Source-files to compile:" $(SRC_FILES)
	@echo "OBJS:" $(PROGRAM_OBJS)
	@echo " "

$(PROGRAM_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $< with flags $(CFLAGS) and defines $(DEFINES_PREFIXED) to $@..."
	@mkdir -p $(dir $@)
	@$(CC) -c $< $(CFLAGS) -o $@ $(DEFINES_PREFIXED)

$(BIN): $(PROGRAM_OBJS) build_score
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $@ $(shell find $(OBJ_DIR)/score -name "*.o") $(PROGRAM_OBJS) $(CFLAGS) $(DEFINES_PREFIXED)

run: $(BIN)
	@$(BIN)

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all build_score build_program clean
