CFLAGS = -Wall -Wextra -pedantic -g -std=c11 -fsanitize=address -Wno-unused-parameter

INCLUDE_DIR = include
INCLUDE_FILES = $(shell find $(INCLUDE_DIR) -name "*.h")

SOURCE_DIR = src
SOURCE_FILES = $(shell find $(SOURCE_DIR) -name "*.c")

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

copy:
ifneq (,$(wildcard ./camuflaje ))
ifneq (,$(wildcard ./backupCamuflaje ))
	rm -r backupCamuflaje
endif
	cp -r ./camuflaje ./backupCamuflaje
else
	@echo "camuflaje not found"
endif

reset:
ifneq (,$(wildcard ./backupCamuflaje ))
ifneq (,$(wildcard ./camuflaje ))
	rm -r camuflaje
endif
	cp -r ./backupCamuflaje ./camuflaje
else
	@echo "backupCamuflaje not found"
endif