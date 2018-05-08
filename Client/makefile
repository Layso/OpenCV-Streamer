leysö: clean compileocv



execName = client

args = 192.168.1.105 3000 1

extraFilesToClear =



compileocv:
	g++ main.cpp client.cpp -o $(execName) `pkg-config opencv --cflags --libs` -pthread



allc: compilec linkc run

allcansi: compilecansi linkc run

allcpp: compilecpp linkcpp run




compilec:
	gcc -c *.c

compilecansi:
	gcc -c *.c -ansi -Wall -pedantic -error

compilecpp:
	g++ -c *.cpp




linkc:
	gcc -o $(execName) *.o

linkcpp:
	g++ -o $(execName) *.o




run:
	./$(execName) $(args)




clean: clear
	rm -f *.o $(execName) $(extraFilesToClear)

clear:
	clear
