.default: all

clean:
	rm -rf barbers prod_cons *.o *.dSYM

barbers: problem_D.o
	gcc -lpthread -std=c99 -Wall -Werror -g -O -o $@ $^

prodCons: problem_A.o
	gcc -lpthread -std=c99 -Wall -Werror -g -O -o $@ $^

%.o: %.cpp
	gcc -lpthread -std=c99 -Wall -Werror -pedantic  -g -O -c $^