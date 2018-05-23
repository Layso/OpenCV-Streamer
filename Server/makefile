leysö: clean compileocv



execName = demo

args = 3001

extraParamsToCompile = `pkg-config --cflags --libs opencv`

extraFilesToClear =




compileocv:
	g++ camshiftdemo.cpp streamer.cpp -o $(execName) `pkg-config opencv --cflags --libs` -pthread




allc: compilec linkc run

allcansi: compilecansi linkc run

allcpp: compilecpp linkcpp run




compilec:
	gcc -c *.c $(extraParamsToCompile)

compilecansi:
	gcc -c *.c $(extraParamsToCompile) -ansi -Wall -pedantic -error

compilecpp:
	g++ -c *.cpp $(extraParamsToCompile)



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
