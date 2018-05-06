leysö: clean compileocv



execName = client

args = 10.1.44.156 3000

extraFilesToClear =



compileocv:
	g++ client.cpp -o client `pkg-config opencv --cflags --libs`



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
