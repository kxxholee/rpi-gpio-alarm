CXX = g++
CXXFLAGS = -std=c++11 $(shell pkg-config opencv4 --cflags) -pthread
LDFLAGS = -ltensorflow-lite $(shell pkg-config opencv4 --libs) -pthread -ldl
INCLUDES = /home/pi/working/ProjectCode/CTesting/tensorflow \
           /home/pi/working/ProjectCode/CTesting/tensorflow/tensorflow/lite/tools/make/downloads/flatbuffers/include
LIBS = /home/pi/working/ProjectCode/CTesting/tensorflow/tensorflow/lite/tools/make/gen/rpi_armv7l/lib
TARGET = train
SRC = $(TARGET).cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) -o $(TARGET) $(SRC) $(addprefix -I,$(INCLUDES)) $(addprefix -L,$(LIBS)) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
