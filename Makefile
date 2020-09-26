NAME = wssolve

SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

ARCH = $(shell g++ -dumpmachine)

ifeq ($(findstring x86_64,$(ARCH)),x86_64)
CCTARGET = -m64
else
ifeq ($(findstring i686,$(ARCH)),i686)
CCTARGET = -m32
else
CCTARGET =
endif
endif

ifeq (, $(shell rm --version))
RM = del
else
RM = rm -f
endif

CFLAGS = -std=c++11 -fPIC

ifeq ($(OS),Windows_NT)
LFLAGS = -L.
endif
LFLAGS += -lzed $(shell pkg-config --cflags --libs tesseract opencv)

CC = g++
LN = g++

default: $(NAME)

$(NAME): $(OBJS)
	$(LN) -o $@ $^ $(LFLAGS)

%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) -o $@ -c $<

src/main.o: src/main.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	$(RM) $(OBJS) $(NAME)

.PHONY: default clean
