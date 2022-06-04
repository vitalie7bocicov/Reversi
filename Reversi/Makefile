all:
	gcc server.c -o sv -lsqlite3 -pthread
	g++ -c rev.cpp
	g++ rev.o -o cl -lsfml-graphics -lsfml-window -lsfml-system
	./sv
clean:
	rm -f sv rev.o 

run:
	./cl 127.0.0.1 2000
