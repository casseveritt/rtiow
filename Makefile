

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
	clang-format -style=file hitable.h > tmp.h
	mv tmp.h hitable.h
	clang-format -style=file quadratic.h > tmp.h
	mv tmp.h quadratic.h
	clang-format -style=file sphere.h > tmp.h
	mv tmp.h sphere.h

clean:
	rm -f *.o
	rm -f rt
	rm -f *~
	rm -f .*~
	rm -f out.png


