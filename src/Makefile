DIR      = .
TARGET   = ${DIR}/Xperimenter

CC       = $(shell wx-config --cxx)

WFLAGS   = -W -Wall -Wextra -Werror -Wfatal-errors

CFLAGS   = -c -std=gnu++11 `wx-config --cxxflags`
CFLAGS  += -I /opt/local/include
#CFLAGS  += -I /usr/include/boost

LDFLAGS = -L /opt/local/lib
#LDFLAGS  = -L /usr/lib/x86_64-linux-gnu/

LDLIBS   = `wx-config --libs core,base,propgrid,adv` # added core,base,propgrid
#LDLIBS  += -lwx_osx_cocoau_propgrid-3.0.0.2.0 # or add core,base,propgrid above
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LDLIBS  += -lboost_system
	LDLIBS  += -lboost_filesystem
	LDLIBS  += -lboost_serialization
	LDLIBS  += -lboost_thread
endif
ifeq ($(UNAME_S),Darwin)
	LDLIBS  += -lboost_system-mt
	LDLIBS  += -lboost_filesystem-mt
	LDLIBS  += -lboost_serialization-mt
	LDLIBS  += -lboost_thread-mt
endif

HDRS := $(wildcard *.h)
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean build info rebuild

all: build

$(TARGET): $(OBJS)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $<

info:
	$(CC) $(CFLAGS) -dM -E - < /dev/null

clean:
	rm -f $(OBJS) $(TARGET)

build: $(TARGET)

rebuild: clean build

print:
	@echo $(CC) $(CFLAGS) $(LDLIBS)
