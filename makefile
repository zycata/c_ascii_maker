CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -Iinclude
LDFLAGS = -lm
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)
TARGET = cascii.exe

ifeq ($(OS),Windows_NT)
	# my laptop has like a weird dependency issue or something where adding -lm causes linker errors
	
    LDFLAGS =
else
    LDFLAGS = -lm
endif

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS)  $(LDFLAGS)  -o $(TARGET)

clang: CC = clang
clang: CFLAGS += -O3 -march=native
clang: clean $(TARGET)

# Release build with optimization
release: CFLAGS += -O3 -flto -march=native
release: LDFLAGS += -flto
release: clean $(TARGET)



clean:
	rm -f $(SRCDIR)/*.o $(TARGET)

.PHONY: clean