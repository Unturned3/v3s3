
LDLIBS += -lVE -lvencoder -lMemAdapter
LDFLAGS += -L$(TARGET_DIR)/usr/lib

all: main

main: conf.h cam.o util.o h264.o
h264.o: conf.h util.o
cam.o: conf.h util.o
util.o: conf.h util.h

clean:
	rm -f $(wildcard *.o)
	rm -f main

