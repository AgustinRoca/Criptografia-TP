CFLAGS = -Wall -Wextra -pedantic -g -std=c11 -fsanitize=address -Wno-unused-parameter

INCLUDE_DIR = include
INCLUDE_FILES = $(shell find $(INCLUDE_DIR) -name *.h)

SOURCE_DIR = src
SOURCE_FILES = $(shell find $(SOURCE_DIR) -name *.c)

OBJECT_DIR = obj
OBJECT_FILES = $(patsubst $(SOURCE_DIR)/%,$(OBJECT_DIR)/%,$(SOURCE_FILES:%.c=%.o))

OUTPUT_FILE = main

all: main

$(OBJECT_DIR)/%.o:$(SOURCE_DIR)/%.c $(INCLUDE_FILES)
	@mkdir -p $(@D)
	$(CC) -I$(INCLUDE_DIR) -c -o $@ $< $(CFLAGS) 

main:$(OBJECT_FILES)
	$(CC) -o $(OUTPUT_FILE) $^ $(CFLAGS)

.PHONY:clean
clean:
	rm -rf $(OUTPUT_FILE) $(OBJECT_DIR)