
WARNFLAGS := -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused

CFLAGS_NOWARN := -O2 -isystem stb -std=c++11
CFLAGS := $(CFLAGS_NOWARN) $(WARNFLAGS)

all: rt

H := $(wildcard *.h)

rt: rt.cpp $(H) stb.o
	g++ ${CFLAGS} rt.cpp stb.o -o rt

stb.o: stb.cpp
	g++ ${CFLAGS_NOWARN} stb.cpp -c

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


