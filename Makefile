# Makefile for PermutationGroup library.

CC := gcc
CFLAGS := --std=c++11 -I../rapidjson/include -fPIC

LIB = PermutationGroup.a

SRCS = Code/FactorGroup.cpp \
	Code/NaturalNumberSet.cpp \
	Code/Permutation.cpp \
	Code/PermutationStream.cpp \
	Code/StabilizerChain.cpp

OBJS = $(SRCS:.cpp=.o)

Code/%.o: Code/%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(LIB): $(OBJS)
	ar -cr $(LIB) $?

clean:
	rm -f $(LIB)
	rm -f $(PYMOD_LIB)
	rm -f $(OBJS)
	rm -f $(PYMOD_OBJS)

PYMOD_LIB = PyPermGroup.pyd

PYMOD_SRCS = PyPermGroup/PyPerm.cpp \
		PyPermGroup/PyPermGroup.cpp \
		PyPermGroup/PyStabChain.cpp

PYMOD_OBJS = $(PYMOD_SRCS:.cpp=.o)

PYTHON_INCS = $(shell python3.6-config --includes)
PYTHON_LIBS = $(shell python3.6-config --libs)

PyPermGroup/%.o: PyPermGroup/%.cpp
	$(CC) -c $(CFLAGS) $< -o $@ -I./Code $(PYTHON_INCS)

$(PYMOD_LIB): $(LIB) $(PYMOD_OBJS)
	gcc -shared -o $(PYMOD_LIB) $? $(PYTHON_LIBS) $(LIB)

pymod: $(PYMOD_LIB)

