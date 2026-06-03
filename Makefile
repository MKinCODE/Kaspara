CXX = g++
CXXFLAGS = -std=c++11 -O2
TARGET = kaspara
SOURCES = main.cpp board.cpp engine.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) $(TARGET).exe
