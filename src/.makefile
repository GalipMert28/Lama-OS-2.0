# Makefile for Project

# Variables
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -std=c99
LDFLAGS = -lpthread -lrt
TARGET = project
SRC_DIR = src
DRIVERS_DIR = $(SRC_DIR)/drivers
SOURCES = $(wildcard $(SRC_DIR)/*.c) \
          $(wildcard $(DRIVERS_DIR)/common/*.c) \
          $(wildcard $(DRIVERS_DIR)/acpi/*.c) \
          $(wildcard $(DRIVERS_DIR)/mouse/*.c) \
          $(wildcard $(DRIVERS_DIR)/usb/*.c) \
          $(wildcard $(DRIVERS_DIR)/network/*.c)
OBJECTS = $(SOURCES:.c=.o)

# Rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/local/bin..."
	@cp $(TARGET) /usr/local/bin/$(TARGET)
	@echo "Installation complete."

uninstall:
	@echo "Removing $(TARGET) from /usr/local/bin..."
	@rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstallation complete."

.PHONY: all clean install uninstall
