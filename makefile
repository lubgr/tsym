
BUILD = build
COMMON = -pedantic -Wall -Wextra -fPIC
COVERAGE = -fprofile-arcs -ftest-coverage

CXXFLAGS = $(COMMON) -Werror=conversion -O0 -g3 -ggdb -I include -I src -I build
CFLAGS = $(COMMON) -Wno-sign-compare -Wno-unused-label -Wno-unused-function -O0 -I $(BUILD) -I src
YFLAGS = -d
LDLIBS = -lCppUTest -ltsym -lstdc++ -lm
LDFLAGS = -L $(BUILD)

ifneq ($(TRLOG), NO)
    USE_TRLOG = -DTSYM_USE_TRLOG
    LD_TRLOG = -ltrlog
endif

CXXFLAGS += $(USE_TRLOG)
CFLAGS += $(USE_TRLOG)
LDLIBS += $(LD_TRLOG)

release: COMMON += -O2 -DNDEBUG
release: CXXFLAGS = $(COMMON) -Werror=conversion $(USE_TRLOG) -I include -I src -I build
release: CFLAGS = $(COMMON) -Wno-sign-compare -Wno-unused-label -Wno-unused-function $(USE_TRLOG) \
    -I $(BUILD) -I src
release: NO_DEBUG_STRINGS = 1
release: COVERAGE =

VERSION = build/version.h
INSTALL_PREFIX = /usr/local

MAJOR = $(shell git describe --abbrev=0 --tags | cut -b 2)
MINOR = $(shell git describe --abbrev=0 --tags | cut -b 4)

LIB_BASENAME = libtsym
LIB_NAME = $(LIB_BASENAME).$(MAJOR).$(MINOR).so
TEST_EXEC = bin/runtests

LIB_TARGET = $(BUILD)/$(LIB_NAME)
LIB_SRC = $(wildcard src/*.cpp)
LIB_OBJ = $(LIB_SRC:src/%.cpp=$(BUILD)/%.o) $(BUILD)/parser.o $(BUILD)/scanner.o
TEST_SRC = $(wildcard test/*.cpp)
TEST_OBJ = $(TEST_SRC:test/%.cpp=$(BUILD)/%.o)

default: tests

tests release: $(TEST_EXEC)

lib: $(LIB_TARGET)

$(TEST_EXEC): $(LIB_TARGET) $(TEST_OBJ)
	$(CXX) -Wl,-rpath $(BUILD) $(COVERAGE) -o $(TEST_EXEC) $(TEST_OBJ) $(LDLIBS) $(LDFLAGS)

$(LIB_TARGET): $(LIB_OBJ)
	$(CXX) -shared $(COVERAGE) -o $(LIB_TARGET) $(LIB_OBJ)
	ln -fs $(LIB_NAME) $(BUILD)/$(LIB_BASENAME).so

$(BUILD)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(COVERAGE) -o $@ -c $<

$(BUILD)/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILD)/%.o: $(BUILD)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD)/%.c: src/%.l $(BUILD)/parser.c
	$(LEX) $(LFLAGS) -o $@ $<

$(BUILD)/%.c: src/%.y
	$(YACC) $(YFLAGS) -o $@ $<

$(LIB_SRC) $(TEST_SRC): $(VERSION)

$(VERSION):
	@echo generate $(VERSION)
	@echo "#define TSYM_VERSION_MAJOR $(MAJOR)" > $(VERSION)
	@echo "#define TSYM_VERSION_MINOR $(MINOR)" >> $(VERSION)
	@echo "#define TSYM_COMMIT \"$(shell git describe)\"" >> $(VERSION)
	@echo "#define TSYM_BRANCH \"$(shell git branch | grep '^\*' | cut -b3-)\"" >> $(VERSION)
	@test -n "$(NO_DEBUG_STRINGS)" ||  echo "#define TSYM_DEBUG_STRINGS" >> $(VERSION)
	@echo "#define TSYM_CPP_COMPILER \"`$(CXX) --version | head -n 1`\"" >> $(VERSION)
	@echo "#define TSYM_CPP_FLAGS \"$(CXXFLAGS)\"" >> $(VERSION)
	@echo "#define TSYM_C_COMPILER \"`$(CC) --version | head -n 1`\"" >> $(VERSION)
	@echo "#define TSYM_C_FLAGS \"$(CFLAGS)\"" >> $(VERSION)
	@echo "#define TSYM_PARSER_GENERATOR \"`$(YACC) --version | head -n 1`\"" >> $(VERSION)
	@echo "#define TSYM_LEXICAL_ANALYZER \"`$(LEX) --version | head -n 1`\"" >> $(VERSION)
	@echo "#define TSYM_BUILD_OS \"$(shell uname -mro)\"" >> $(VERSION)
	@echo "#define TSYM_BUILD_DATE \"$(shell date +'%d. %b. %Y, %R %Z')\"" >> $(VERSION)


install: lib
	install -m 644 -Dt $(INSTALL_PREFIX)/include/tsym $(VERSION) include/*
	install -m 644 $(LIB_TARGET) $(INSTALL_PREFIX)/lib/
	ln -sf $(INSTALL_PREFIX)/lib/$(LIB_NAME) $(INSTALL_PREFIX)/lib/$(LIB_BASENAME).so

uninstall:
	rm -f $(INSTALL_PREFIX)/lib/$(LIB_BASENAME).*
	rm -rf $(INSTALL_PREFIX)/include/tsym

test: $(TEST_EXEC)
	@$(TEST_EXEC)

clean:
	$(RM) $(BUILD)/*
	$(RM) $(TEST_EXEC)
	$(RM) doc/coverage/*

.PHONY: default clean release lib test tests install uninstall
