
CXXFLAGS ?= -O0 -g3 -ggdb -pedantic -Wall -Wextra -Werror=conversion -fprofile-arcs -ftest-coverage -fPIC
INCLUDE = -I include -I src

MAJOR = $(shell grep 'major *=' include/version.h | grep -o '[0-9]\+')
MINOR = $(shell grep 'minor *=' include/version.h | grep -o '[0-9]\+')
VERSION = $(MAJOR).$(MINOR)

LIB_BASENAME = libtsym
LIB_NAME = $(LIB_BASENAME).$(VERSION).so
LIB_TARGET = build/$(LIB_NAME)

SRC = $(wildcard src/*.cpp)
OBJ = $(foreach src, $(SRC),$(patsubst src/%,build/%,$(patsubst %.cpp,%.o,$(src))))

TEST_EXEC = bin/runtests
TESTSRC = $(wildcard test/*.cpp)
TESTOBJ = $(foreach src, $(TESTSRC), $(patsubst test/%,build/%,$(patsubst %.cpp,%.o,$(src))))

INSTALL_INCLUDEDIR = /usr/local/include/tsym
INSTALL_LIBDIR = /usr/local/lib

tests: LDLIBS += -lCppUTest -ltsym -lstdc++ -lm
tests: LDFLAGS += -L build
release: CXXFLAGS = -O2 -pedantic -Wall -Wextra -Werror=conversion -fPIC -DNDEBUG

ifneq ($(TRLOG), NO)
    CXXFLAGS += -DUSE_TRLOG
    LDLIBS += -ltrlog
endif

define COMPILE
$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<
endef

default: tests

release: tests

lib: $(LIB_TARGET)

tests: $(TEST_EXEC)

$(LIB_TARGET): $(OBJ)
	$(CXX) -shared -fprofile-arcs -o $(LIB_TARGET) $(OBJ)
	ln -fs $(LIB_NAME) build/$(LIB_BASENAME).so

build/%.o: src/%.cpp
	$(COMPILE)

$(TEST_EXEC): $(LIB_TARGET) $(TESTOBJ)
	$(CXX) -Wl,-rpath build -fprofile-arcs -o $(TEST_EXEC) $(TESTOBJ) $(LDLIBS) $(LDFLAGS)

build/%.o: test/%.cpp
	$(COMPILE)

install: lib
	rsync -r --delete include/ $(INSTALL_INCLUDEDIR)
	cp $(LIB_TARGET) $(INSTALL_LIBDIR)
	ln -sf $(INSTALL_LIBDIR)/$(LIB_NAME) $(INSTALL_LIBDIR)/$(LIB_BASENAME).so
	chmod 755 $(INSTALL_INCLUDEDIR)
	chmod 644 $(INSTALL_INCLUDEDIR)/* $(INSTALL_LIBDIR)/$(LIB_NAME)

uninstall:
	rm -f $(INSTALL_LIBDIR)/$(LIB_BASENAME)*
	rm -rf $(INSTALL_INCLUDE)

test: default
	@$(TEST_EXEC)

clean:
	$(RM) build/*
	$(RM) doc/coverage/*
	$(RM) $(TEST_EXEC)

.PHONY: clean default release lib test tests install
