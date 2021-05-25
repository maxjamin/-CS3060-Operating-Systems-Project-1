proc1: p4.c
		gcc -o p4 p4.c -pthread

clean:
		rm *.o p4 a.out
