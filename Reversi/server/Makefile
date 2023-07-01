CC = gcc
CFLAGS = -Wall -Wextra -g
LFLAGS = -pthread -lsqlite3

OUTPUT = output
SRC = ./src
INCLUDE = include
LIB = lib

ifeq ($(OS),Windows_NT)
MAIN = main.exe
SOURCEDIRS = $(SRC)
INCLUDEDIRS = $(INCLUDE)
LIBDIRS = $(LIB)
FIXPATH = $(subst /,\,$1)
RM = del /q /f
MD = mkdir
else
MAIN = main
SOURCEDIRS = $(shell find $(SRC) -type d)
INCLUDEDIRS = $(shell find $(INCLUDE) -type d)
LIBDIRS = $(shell find $(LIB) -type d)
FIXPATH = $1
RM = rm -f
MD = mkdir -p
endif

INCLUDES = $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
LIBS = $(patsubst %,-L%, $(LIBDIRS:%/=%))

SOURCES = $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))
OBJECTS = $(patsubst $(SRC)/%.c,$(OUTPUT)/%.o,$(SOURCES))
DEPS = $(OBJECTS:.o=.d)

OUTPUTMAIN = $(call FIXPATH,$(OUTPUT)/$(MAIN))

all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(OBJECTS) $(LFLAGS)

# include all .d files
-include $(DEPS)

$(OUTPUT)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -MMD $< -o $@

.PHONY: clean
clean:
	$(RM) $(OUTPUTMAIN)
	$(RM) $(call FIXPATH,$(OBJECTS))
	$(RM) $(call FIXPATH,$(DEPS))
	@echo Cleanup complete!

run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!