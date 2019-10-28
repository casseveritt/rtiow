

all: rt


rt: rt.cpp
	g++ -std=c++11 rt.cpp -o rt


clean:
	rm -f *.o
	rm -f rt
	rm -f *~
	rm -f .*~
	rm -f out.png


