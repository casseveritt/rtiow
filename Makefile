
WARNFLAGS := -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused

CFLAGS := -O2 -isystem stb -std=c++11 $(WARNFLAGS)

all: rt

H := $(filter-out $(wildcard stb_*.h), $(wildcard *.h) )

rt: rt.cpp $(H)
	g++ ${CFLAGS} rt.cpp -o rt

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


