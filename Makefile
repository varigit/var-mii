all: main.cpp
	$(CXX) $(CXXFLAGS) -Og main.cpp -g -o var-mii
clean:
	rm -f var-mii
