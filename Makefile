CXXFLAGS = -fPIC -O3 -std=c++0x -Wall -Wconversion -Werror -Wextra \
	   -Wformat=2 -Wno-unused-function -Wpointer-arith \
	   -Wstrict-overflow=5

CXX = g++

all:	clover.cc
	$(CXX) $(CXXFLAGS) clover.cc -o clover

clean:
	rm -f *.o clover

purge:
	rm -f *~*
