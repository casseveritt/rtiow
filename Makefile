

all: rt


rt: rt.cpp
	g++ -g -std=c++11 rt.cpp -o rt

format:
	clang-format -style=file rt.cpp > tmp.cpp
	mv tmp.cpp rt.cpp
	clang-format -style=file linear.h > tmp.h
	mv tmp.h linear.h
	clang-format -style=file ray.h > tmp.h
	mv tmp.h ray.h

clean:
	rm -f *.o
	rm -f rt
	rm -f *~
	rm -f .*~
	rm -f out.png


