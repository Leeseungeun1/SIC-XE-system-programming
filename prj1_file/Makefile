20171290.out: slice.o subfunc.o command.o 20171290.o
	gcc -o 20171290.out -Wall slice.o subfunc.o command.o 20171290.o

slice.o: 20171290.h slice.h slice.c
	gcc -c -Wall slice.c
subfunc.o: 20171290.h subfunc.h subfunc.c
	gcc -c -Wall subfunc.c
command.o: command.c command.h 
	gcc -c -Wall command.c 
20171290.o: 20171290.c
	gcc -c -Wall 20171290.c 

clean:
	rm *.o *.out
