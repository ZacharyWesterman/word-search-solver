NAME = wssolve-img
TXTNAME = wssolve-txt

SRCS = $(wildcard src/cv/*.cpp src/shared/*.cpp)
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

CFLAGS = -std=c++17 -fPIC
LFLAGS += -lzed $(shell pkg-config --cflags --libs tesseract opencv)

CC = g++
LN = g++

DICTOUT = data/us.dict
DICTIN = /usr/share/dict/words
UTILNAME = src/utils/dict

default: $(NAME) $(DICTOUT) $(UTILNAME) $(TXTNAME)

$(NAME): src/main.o $(OBJS) $(DICTOUT)
	$(LN) -o $@ $< $(OBJS) $(LFLAGS)

%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) -o $@ -c $<

src/main.o: src/main.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

src/solve.o: src/solve.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

$(TXTNAME): src/solve.o src/shared/wordSearch.o $(DICTOUT)
	$(LN) -o $@ $< src/shared/wordSearch.o -lzed

$(UTILNAME): $(UTILNAME).cpp
	$(CC) $(CFLAGS) $< -lzed -o $@

$(DICTOUT): $(UTILNAME) $(DICTIN)
	mkdir -p data
	chmod +x $<
	./$^

clean:
	$(RM) $(NAME) $(DICTOUT) $(UTILNAME) $(TXTNAME) src/*.o src/utils/*.o $(OBJS)

.PHONY: default clean
