

all: rt

H := $(wildcard *.h)

rt: rt.cpp $(H)
	g++ -O2 -std=c++11 rt.cpp -o rt

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
	clang-format -style=file camera.h > tmp.h
	mv tmp.h camera.h
	clang-format -style=file env.h > tmp.h
	mv tmp.h env.h

clean:
	rm -f *.o
	rm -f rt
	rm -f *~
	rm -f .*~
	rm -f out.png


