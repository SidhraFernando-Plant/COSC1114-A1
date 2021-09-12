.default: all

clean:
	rm -rf barbers prod_cons *.o *.dSYM

barbers: problem_D.o
	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^

prod_cons: problem_A.o
	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^

%.o: %.cpp
	g++ -Wall -Werror -pedantic -std=c++17 -g -O -c $^