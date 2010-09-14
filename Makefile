CFLAGS  = -g -std=c++0x -O3
SOURCES = image.cpp rotation_engine.cpp program.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = rot

all: $(SOURCES) $(EXECUTABLE)
 
$(EXECUTABLE): $(OBJECTS)  
	g++ $(OBJECTS) -o $@

%.o: %.cpp
	g++ $(CFLAGS) -c $< -o $@ 

clean:
	rm -rf *.o $(EXECUTABLE)

