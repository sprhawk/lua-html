CC=gcc
CFLAGS=-c -I/opt/local/include -I/opt/local/include/libxml2 -I/opt/local/include/lua
LDFLAGS=-L/opt/local/lib

AR=ar
ARFLAGS=-ru

#SOURCES=html.c
OBJECTS=html.o

all:html.so

html.a:$(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

html.so:$(OBJECTS)
	$(CC) $(LDFLAGS) -shared -Wall -lxml2 -undefined dynamic_lookup -o $@ $<

html.dylib:$(OBJECTS)
	$(CC) $(LDFLAGS) -dynamiclib -Wall -lxml2 -llua -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -o $@ $<



clean:
	rm -f *.so *.o *.a *.dylib

