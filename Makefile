prog:	partie1.cpp partie2.cpp tests.cpp main.cpp
	g++ 	partie1.cpp partie2.cpp tests.cpp main.cpp -o prog 

clean:
	rm prog
