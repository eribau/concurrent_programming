IDIR=src/include
CC=gcc
CFLAGS=-fopenmp -I$(IDIR)

ODIR=obj
SDIR=src
TDIR=tests

LIBS=-lm

_DEPS=
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=aux.o nSqr_seq.o nSqr_par.o driver.o quadtree.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

all: test driver

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

driver: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

test: $(ODIR)/test.o $(ODIR)/quadtree.o $(ODIR)/aux.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
