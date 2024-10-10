CPP     = g++
SRCDIR  = src
INCDIR  = include
INCDIR2 = /usr/include
#CFLAGS  = -O0 -g -std=c++11 -DDEBUG
CFLAGS  = -O2 -s -w -std=c++11 -DINSECURE -DNAIVE8

# Security Mode
# -DINSECURE
# -DSECURE

# Algorithm and Difficulty
# -DNAIVE8
# -DFLETCHER16
# -DFLETCHER32
# -DJB2
# -DFLETCHER64

# Debug Mode
# -DDEBUG

LFLAGS  = -I$(INCDIR) -I$(INCDIR2) -lsqlite3 -lrt -lcrypto
UPX = upx -qq

.SUFFIXES: .o .hpp .cpp

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(CFLAGS) -I$(INCDIR) -I$(INCDIR2) -c $< -o $@

HEADERS = $(wildcard $(INCDIR)/*.hpp $(INCDIR2)/sqlite*.h)

SRCS = $(wildcard $(SRCDIR)/*.cpp)

OBJS = $(SRCS:.cpp=.o)

all: blockring

blockring: $(HEADERS) $(OBJS)
	@$(CPP) $(CFLAGS) $(OBJS) -o $@ $(LFLAGS)
	@$(UPX) $@
	@echo "blockring compiled successfully"

clean_objects:
	@rm -f $(SRCDIR)/*.o

clean: clean_objects
	@rm -f blockring
