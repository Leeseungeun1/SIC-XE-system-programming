20171290.out: slice.o subfunc.o command.o pass1.o 20171290.o
	gcc -o 20171290.out -Wall slice.o subfunc.o command.o pass1.o 20171290.o

slice.o: 20171290.h slice.h slice.c
	gcc -c -Wall slice.c
subfunc.o: 20171290.h subfunc.h subfunc.c
	gcc -c -Wall subfunc.c
command.o: 20171290.h command.c command.h 
	gcc -c -Wall command.c 
pass1.o: 20171290.h pass1.h pass1.c
	gcc -c -Wall pass1.c
20171290.o: 20171290.h 20171290.c
	gcc -c -Wall 20171290.c 

clean:
	rm *.o *.out
