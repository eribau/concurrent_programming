IDIR=src/include
CC=gcc
CFLAGS=-fopenmp -I$(IDIR) -O3

ODIR=obj
SDIR=src
TDIR=tests

LIBS=-lm

_DEPS=
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=aux.o nSqr_seq.o nSqr_par.o quadtree.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

all: test nSqr_seq nSqr_par barnesHut_seq barnesHut_par

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

nSqr_seq: $(ODIR)/nSqr_seq.o $(ODIR)/aux.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

nSqr_par: $(ODIR)/nSqr_par.o $(ODIR)/aux.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# driver: $(OBJ)
# 	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

barnesHut_seq: $(ODIR)/barnesHut_seq.o $(ODIR)/quadtree.o $(ODIR)/aux.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

barnesHut_par: $(ODIR)/barnesHut_par.o $(ODIR)/quadtree.o $(ODIR)/aux.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

test: $(ODIR)/test.o $(ODIR)/quadtree.o $(ODIR)/aux.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
