CXX      := ccache g++
CC       := ccache gcc
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -O3 -g -MMD -MP
CFLAGS   := -std=c11 -O3 -Wall -Wextra -MMD -MP
LDFLAGS  := -lncurses -lutf8proc

SRCS     := main.cpp columns.cpp display.cpp flag.cpp unicode_to_ascii.cpp
CSRCS    := anyascii.c

OBJS     := $(SRCS:%.cpp=build/cpp/%.o)
COBJS    := $(CSRCS:%.c=build/c/%.o)
DEPS     := $(OBJS:.o=.d) $(COBJS:.o=/d)

all: nmenu_plus

nmenu_plus: $(OBJS) $(COBJS)
	$(CXX) $(OBJS) $(COBJS) $(LDFLAGS) -o $@

build/c/%.o: %.c  | build
	$(CC) $(CFLAGS) -c $< -o $@

build/cpp/%.o: %.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build/c build/cpp

-include $(DEPS)

clean:
	rm -rf build nmenu_plus
.PHONY: all clean
