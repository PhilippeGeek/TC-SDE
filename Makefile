CC=gcc
CFLAGS=-c -Iheaders -Ilib -O3
LDFLAGS=

headers = $(wildcard headers/*.h)
lib = $(wildcard lib/*.h)

all: coordinateur generateur_trafic generateur_trafic_prioritaire feux

O_FOR_ALL = lib.o voiture.o

coordinateur: coordinateur.o ${O_FOR_ALL}
	$(CC) $(LDFLAGS) coordinateur.o ${O_FOR_ALL} -o $@

generateur_trafic: generateur_trafic.o ${O_FOR_ALL}
	$(CC) $(LDFLAGS) generateur_trafic.o ${O_FOR_ALL} -o $@

generateur_trafic_prioritaire: generateur_trafic_prioritaire.o ${O_FOR_ALL}
	$(CC) $(LDFLAGS) generateur_trafic_prioritaire.o ${O_FOR_ALL} -o $@

feux: feux.o ${O_FOR_ALL}
	$(CC) $(LDFLAGS) feux.o ${O_FOR_ALL} -o $@


%.o: %.c ${headers} ${libs}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o coordinateur generateur_trafic generateur_trafic_prioritaire feux