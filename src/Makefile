LIBS=-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_video
LIB_DIR=-L/usr/local/lib
INCLUDE_DIR=-I/usr/local/include

CC=g++

CXXFLAGS=$(LIBS) $(LIB_DIR) $(INCLUDE_DIR)
TARGET=opencv
$(TARGET):
	$(CC) main.cpp -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGET)
	echo "Disclean!"
