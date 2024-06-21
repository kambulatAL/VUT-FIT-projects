# author: Kambulat Alakaev(xalaka00)
# Projekt: IMS

EXEC = main
CXX = g++
CXXFLAGS = -std=c++17 -Wall
ZIP_FILE = 06_xalaka00.zip

all: $(EXEC)

$(EXEC): main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lsimlib 

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $^

.PHONY: 
clean:
	rm -f $(EXEC) *.o $(ZIP_FILE)

run: ./main
	./main

zip:
	zip -r $(ZIP_FILE) main.cpp Makefile dokumentace.pdf experiments simlib.h