# File: Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./rapidjson/include
LDFLAGS = -lcurl
TARGET = graph_crawler
SRC = graph_crawler.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

