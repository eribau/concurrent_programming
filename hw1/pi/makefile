IDIR=src/include
CC=gcc
CFLAGS=-lpthread

ODIR=obj
SDIR=src

LIBS=-lm

_DEPS=pi.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=pi.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

pi: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
