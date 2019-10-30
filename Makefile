

all: rt

H := $(filter-out $(wildcard stb_*.h), $(wildcard *.h) )

rt: rt.cpp $(H)
	g++ -O2 -std=c++11 rt.cpp -o rt

format: format_h
	clang-format -style=file rt.cpp > tmp.cpp
	mv tmp.cpp rt.cpp

format_h: $(H)
	for h in $^; do clang-format -style=file $$h > tmp.h; mv tmp.h $$h; done


clean:
	rm -f *.o
	rm -f rt
	rm -f *~
	rm -f .*~
	rm -f out.png


