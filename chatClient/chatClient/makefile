client : main.o tool.o list.o
	cc -o client main.o tool.o list.o -lpthread
main.o : main.c
	cc -c main.c
tool.o : tool.c
	cc -c tool.c
list.o : list.c
	cc -c list.c
clean : 
	rm *.o

