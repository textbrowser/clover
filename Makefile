CXX = g++

CXXFLAGS = -O3 \
	   -Wall \
	   -Wconversion \
	   -Werror \
	   -Wextra \
	   -Wformat=2 \
	   -Wno-unused-function \
	   -Wpointer-arith \
	   -Wstrict-overflow=5 \
	   -fPIC \
	   -std=c++11

all:	clover.cc
	$(CXX) $(CXXFLAGS) clover.cc -o clover

clean:
	rm -f *.o clover

purge:
	rm -f *~*
