CXX = g++

CXXFLAGS = -O3 \
	   -Wall \
	   -Wconversion \
	   -Werror \
	   -Wextra \
	   -Wformat=2 \
	   -Wformat-security \
	   -Wno-unused-function \
	   -Wno-vla \
	   -Wpointer-arith \
	   -Wsign-conversion \
	   -Wstrict-overflow=5 \
	   -fPIC \
	   -pedantic \
	   -std=c++11

all:	clover.cc
	$(CXX) $(CXXFLAGS) clover.cc -o clover

clean:
	rm -f *.o clover

purge:
	rm -f *~*
