CXX = g++
CXXFLAGS = -Wall -std=c++11

myshell: main.cpp Shell.cpp
	$(CXX) $(CXXFLAGS) main.cpp Shell.cpp -o myshell

clean:
	rm -f myshell