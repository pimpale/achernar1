TARGET_EXEC ?= lasagna

SRC_DIRS ?= ./src
BUILD_DIR ?= ./obj

SRCS := $(shell find $(SRC_DIRS) -type f -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

LDFLAGS := 

CC := clang
CPPFLAGS ?= -std=c11 -MMD -MP -O0 -g3 -Wall -Weverything
# CC := tcc
# CPPFLAGS ?= $(INC_FLAGS) -Wall -Wpedantic 
# CC := gcc
# CPPFLAGS ?= $(INC_FLAGS) -std=gnu11 -MMD -MP -O0 -pg -Wall -Wpedantic 

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
