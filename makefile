
BUILD = build
COMMON = -pedantic -Wall -Wextra -fPIC
COVERAGE = -fprofile-arcs -ftest-coverage

CXXFLAGS = $(COMMON) -Werror=conversion -O0 -g3 -ggdb $(COVERAGE) -I include -I src
CFLAGS = $(COMMON) -Wno-unused-function -O0 -I $(BUILD) -I src
YFLAGS = -y -d
LDLIBS = -lCppUTest -ltsym -lstdc++ -lm
LDFLAGS = -L $(BUILD)

ifneq ($(TRLOG), NO)
    USE_TRLOG = -DUSE_TRLOG
    LD_TRLOG = -ltrlog
endif

CXXFLAGS += $(USE_TRLOG)
CFLAGS += $(USE_TRLOG)
LDFLAGS += $(LD_TRLOG)

release: COMMON += -O2 -DNDEBUG
release: CXXFLAGS = $(COMMON) -Werror=conversion $(USE_TRLOG) -I include -I src
release: CFLAGS = $(COMMON) -Wno-unused-function $(USE_TRLOG) -I $(BUILD)
release: COVERAGE =

INSTALL_PREFIX = /usr/local

MAJOR = $(shell grep 'major *=' include/version.h | grep -o '[0-9]\+')
MINOR = $(shell grep 'minor *=' include/version.h | grep -o '[0-9]\+')

LIB_BASENAME = libtsym
LIB_NAME = $(LIB_BASENAME).$(MAJOR).$(MINOR).so
TEST_EXEC = bin/runtests

LIB_TARGET = $(BUILD)/$(LIB_NAME)
LIB_SRC = $(wildcard src/*.cpp)
LIB_OBJ = $(LIB_SRC:src/%.cpp=$(BUILD)/%.o) $(BUILD)/parser.o $(BUILD)/scanner.o
TEST_SRC = $(wildcard test/*.cpp)
TEST_OBJ = $(TEST_SRC:test/%.cpp=$(BUILD)/%.o)

VPATH = src/ test/

tests release: $(TEST_EXEC)

lib: $(LIB_TARGET)

$(TEST_EXEC): $(LIB_TARGET) $(TEST_OBJ)
	$(CXX) -Wl,-rpath $(BUILD) $(COVERAGE) -o $(TEST_EXEC) $(TEST_OBJ) $(LDLIBS) $(LDFLAGS)

$(LIB_TARGET): $(LIB_OBJ)
	$(CXX) -shared $(COVERAGE) -o $(LIB_TARGET) $(LIB_OBJ)
	ln -fs $(LIB_NAME) $(BUILD)/$(LIB_BASENAME).so

$(BUILD)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILD)/%.c: %.l $(BUILD)/parser.c
	$(LEX) $(LFLAGS) -o $@ $<

$(BUILD)/%.c: %.y
	$(YACC) $(YFLAGS) -o $@ $<

install: lib
	rsync -r --delete include/ $(INSTALL_PREFIX)/include/tsym
	cp $(LIB_TARGET) $(INSTALL_PREFIX)/lib/
	ln -sf $(INSTALL_PREFIX)/lib/$(LIB_NAME) $(INSTALL_PREFIX)/lib/$(LIB_BASENAME).so
	chmod 755 $(INSTALL_PREFIX)/include/tsym
	chmod 644 $(INSTALL_PREFIX)/include/tsym* $(INSTALL_PREFIX)/lib/$(LIB_NAME)

uninstall:
	rm -f $(INSTALL_LIBDIR)/$(LIB_BASENAME)*
	rm -rf $(INSTALL_INCLUDE)

test: $(TEST_EXEC)
	@$(TEST_EXEC)

clean:
	$(RM) $(BUILD)/*
	$(RM) $(TEST_EXEC)
	$(RM) doc/coverage/*

.PHONY: clean release lib test tests install
