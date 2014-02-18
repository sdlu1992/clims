main : main.o draw.o list.o
	gcc -o main -Wall -g main.o draw.o list.o -lncursesw -lformw

main.o : main.c drawnlist.h
	gcc -c -g main.c 

draw.o : draw.c drawnlist.h
	gcc -c -g draw.c 

list.o : list.c drawnlist.h
	gcc -c -g list.c 

clean :
	rm edit main.o draw.o list.o
