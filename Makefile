all:
	bison -d parser.yy
	g++ parser.tab.cc -c -O3 -flto -Wall -DYYERROR_VERBOSE -std=c++11 `llvm-config --cppflags --libs all --ldflags --system-libs`
	flex  scanner.l
	g++ lex.yy.c -c -O3 -flto -Wall -DYYERROR_VERBOSE -std=c++11 `llvm-config --cppflags --libs all --ldflags --system-libs`
	g++ -o compiler parser.tab.o lex.yy.o -O3 -flto -Wall -DYYERROR_VERBOSE -std=c++11 `llvm-config --cppflags --libs all --ldflags --system-libs` -lfl
clean:
	rm lex.yy.c lex.yy.o parser.tab.cc parser.tab.hh parser.tab.o
