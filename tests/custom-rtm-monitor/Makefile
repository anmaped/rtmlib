
DEBUG:=-DRTMLIB_ENABLE_DEBUG_RMTLD3 -DRTMLIB_ENABLE_DEBUGV_RMTLD3  -DRTMLIB_DEBUG=3
C:=gcc -I./../../src/ -std=gnu++11 -lstdc++ -latomic -fno-rtti -fno-exceptions $(DEBUG)

all:
	$C -o test1 rtm-monitor-test1.cpp
	$C -o test2 rtm-monitor-test2.cpp

clean:
	rm -f test1 test2
