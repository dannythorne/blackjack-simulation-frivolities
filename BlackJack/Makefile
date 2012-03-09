
CFLAGS =
CFLAGS = -O2
#CFLAGS+=-g

a.exe: main.cc blackjack.o
	g++ $(CFLAGS) main.cc blackjack.o

blackjack.o: blackjack.h blackjack.cc
	g++ -c $(CFLAGS) blackjack.cc

clean:
	/bin/rm -f a.exe* *.o
