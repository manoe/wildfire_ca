all: wf wf_test

wf_test: main.cpp wf.h
	g++ -Wall -Wdouble-promotion -Wc++11-compat -Werror -Wextra -pedantic -DWF_UT main.cpp -o wf_test
	./wf_test

wf: main.cpp wf.h
	g++ -Wall -Wdouble-promotion -Wc++11-compat -Werror -Wextra -pedantic main.cpp -o wf

clean:
	rm -rf wf
