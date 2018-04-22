CC=gcc
CFLAGS=-ggdb3 -c -Wall -std=gnu99
LDFLAGS=-pthread
SOURCES=thpool.c example.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=example

all: $(SOURCES) $(EXECUTABLE)

debug: CFLAGS += -DDEBUG -g -DDEBUG_THPOOL
debug: all

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
