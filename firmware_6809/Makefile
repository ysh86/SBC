TARGET_EXEC ?= main.hex

TARGET_CHIP ?= 18F47Q84

ROM_DIR ?= ../ROM
ROM_NAME ?= ROM16KB
ROM_HEADER ?= $(ROM_DIR)/$(ROM_NAME).h

BUILD_DIR ?= ./build
SRC_DIRS ?= ./

SRCS := $(shell find -L $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.obj)
DEPS := $(OBJS:.obj=.d)

CC := xc8
CFLAGS := --std=c99
CFLAGS += --chip=$(TARGET_CHIP)

# libs
HEX2BIN := ftPIC.exe -i


.PHONY: all
all: $(TARGET_EXEC)

# hex2bin
$(TARGET_EXEC): $(BUILD_DIR)/$(TARGET_EXEC)
	$(CP) -p $< $@
	$(HEX2BIN) $@

$(BUILD_DIR)/$(TARGET_EXEC): $(ROM_HEADER) $(OBJS)

# ROM
$(ROM_HEADER): $(ROM_DIR)/$(ROM_NAME).bin
	cat $< | xxd -i -c 16 > $@

# c source
$(BUILD_DIR)/%.obj: %.c
	$(MKDIR_P) $(dir $@)
	cd $(dir $@); $(CC) $(CPPFLAGS) $(CFLAGS) ../$<


.PHONY: clean
clean:
	$(RM) -r $(TARGET_EXEC) $(OBJS) $(DEPS) $(BUILD_DIR)

-include $(DEPS)

CP ?= cp
RM ?= rm -f
MKDIR_P ?= mkdir -p
