CC=gcc
ICC=icc
MPICC=mpicc
MPIICC=mpiicc

CFLAGS=-O3
OMPFLAGS=-fopenmp
MPIFLAGS=

HEAT_SOURCES=heat.c file-reader.c
SERIAL_SOURCES=main-serial.c $(HEAT_SOURCES)
MPI_SOURCES=main-mpi.c $(HEAT_SOURCES)

.PHONY: all gccserial gcccomplete iccserial icccomplete clean

all: gccserial gcccomplete iccserial icccomplete

gccserial:
	$(CC) $(CFLAGS) $(OMPFLAGS) $(SERIAL_SOURCES) -o heat-omp-gcc

gcccomplete:
	$(MPICC) $(CFLAGS) $(MPIFLAGS) $(MPI_SOURCES) -o heat-complete-gcc

iccserial:
	$(ICC) $(CFLAGS) $(OMPFLAGS) $(SERIAL_SOURCES) -o heat-omp-icc

icccomplete:
	$(MPIICC) $(CFLAGS) $(MPIFLAGS) $(MPI_SOURCES) -o heat-complete-icc

clean:
	rm -f heat-omp-gcc heat-complete-gcc heat-omp-icc heat-complete-icc