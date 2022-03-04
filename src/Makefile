all: wf

test: main.cpp wf.h
	g++ -Wall -Wdouble-promotion -Wc++11-compat -Werror -Wextra -pedantic -DWF_UT main.cpp -o wf
	./wf

wf: main.cpp wf.h
	g++ -Wall -Wdouble-promotion -Wc++11-compat -Werror -Wextra -pedantic main.cpp -o wf

clean:
	rm -rf wf
