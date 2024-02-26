CC=gcc
CFLAGS= -lm -Wall -fopenmp -Ofast

# Cibles pour chaque exécutable
all: benchmark

hl2: hl2.o xxhash.o common_hll.o
	$(CC) -o hl2 hl2.o xxhash.o common_hll.o $(CFLAGS)

hl3: hl3.o xxhash.o common_hll.o
	$(CC) -o hl3 hl3.o xxhash.o common_hll.o $(CFLAGS)

hl4: hl4.o xxhash.o common_hll.o
	$(CC) -o hl4 hl4.o xxhash.o common_hll.o $(CFLAGS)

benchmark: benchmark.o hl2.o hl3.o hl4.o xxhash.o common_hll.o
	$(CC) -o benchmark benchmark.o hl2.o hl3.o hl4.o xxhash.o common_hll.o $(CFLAGS)

# Règle pour compiler les fichiers source en fichiers objets
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Nettoyer les fichiers compilés
clean:
	rm -f *.o hl2 hl3 hl4 benchmark
