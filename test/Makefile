test2 : classes.o
	g++ ../obj/classes.o ../test/test2.cpp -std=c++11 -o ../test/test
test : classes.o
	g++ ../obj/classes.o ../test/test.cpp -std=c++11 -o ../test/test
classes.o :
	g++ ../source/classes.cpp -c -lm -std=c++11 -o ../obj/classes.o
clean :
	rm ../obj/classes.o
