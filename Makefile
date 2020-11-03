smu_shell: support.o smu_shell.o
	gcc -o smu_shell support.o smu_shell.o
support.o: support.c support.h
	gcc -c support.c
smu_shell.o: smu_shell.c support.h
	gcc -c smu_shell.c
clean:
	\rm *.o smu_shell