20171290.out: slice.o subfunc.o command.o assemble.o write.o 20171290.o
	gcc -o 20171290.out -Wall slice.o subfunc.o command.o assemble.o write.o 20171290.o

slice.o: 20171290.h slice.h slice.c
	gcc -c -Wall slice.c
subfunc.o: 20171290.h subfunc.h subfunc.c
	gcc -c -Wall subfunc.c
command.o: 20171290.h command.c command.h 
	gcc -c -Wall command.c 
assemble.o: 20171290.h assemble.h assemble.c
	gcc -c -Wall assemble.c
write.o: 20171290.h write.h write.c
	gcc -c -Wall write.c
20171290.o: 20171290.h 20171290.c
	gcc -c -Wall 20171290.c 

clean:
	rm *.o *.out
