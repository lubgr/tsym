
include $(wildcard makefile.in Makefile.in)

BUILD ?= build
CXXFLAGS ?= -O2 -g -pedantic -Wall -Wextra -Werror=conversion -DNDEBUG -std=c++11
CFLAGS ?= -pedantic -Wall -Wextra -Wno-sign-compare -Wno-unused-label -Wno-unused-function \
	  -Wno-unneeded-internal-declaration -Wno-write-strings
CPPFLAGS += -I src
COVERAGE ?=
LIBS ?= -lgmp
LDFLAGS += -L $(BUILD) -Wl,-rpath $(BUILD)
LEX ?= flex
YACC ?= bison
TAGS ?= $(BUILD)/tags
SO ?= so
PREFIX ?= /usr/local

BUILDINFO = $(BUILD)/buildinfo.h
COMMIT = $(shell git log --pretty=format:%h -n 1 2> /dev/null || echo unknown)
BRANCH = $(shell git branch | grep '^\*' | cut -b3- || echo unknown)

NAME = tsym
TEST_EXEC = $(BUILD)/runtests
LIB_TARGET = $(BUILD)/lib$(NAME).$(SO)
LIB_HEADER = $(BUILD)/$(NAME).h
PUBLIC_HEADER = globals logger matrix var vector version

LIB_SRC = $(wildcard src/*.cpp)
LIB_PARSER_H = $(BUILD)/src/parser.h
LIB_OBJ = $(LIB_SRC:%.cpp=$(BUILD)/%.o) $(BUILD)/src/scanner.o $(LIB_PARSER_H:%.h=%.o)
TEST_SRC = $(wildcard test/*.cpp)
TEST_OBJ = $(TEST_SRC:%.cpp=$(BUILD)/%.o)
DEPS = $(LIB_OBJ:%.o=%.d) $(TEST_OBJ:%.o=%.d)

default: tests
tests: $(LIB_TARGET) $(TEST_EXEC)
lib: $(LIB_TARGET)
tags: $(TAGS)

$(TEST_EXEC): $(LIB_TARGET) $(TEST_OBJ)
	$(CXX) -o $(TEST_EXEC) $(COVERAGE) $(TEST_OBJ) $(LDFLAGS) $(LIBS) -lCppUTest -l$(NAME)

$(LIB_TARGET): $(LIB_OBJ) $(BUILDINFO)
	$(CXX) -shared -o $(LIB_TARGET) $(COVERAGE) $(LIB_OBJ) $(LIBS)

-include $(DEPS)

$(BUILD)/src/%.o: src/%.cpp | $(BUILD)/src
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COVERAGE) -fPIC -o $@ -c $<

$(BUILD)/src/%.o: $(BUILD)/src/%.c $(LIB_PARSER_H)
	$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -o $@ -c $<

$(LIB_PARSER_H): $(LIB_PARSER_H:%.h=%.c)

$(BUILD)/src/%.c: src/%.l | $(BUILD)/src
	$(LEX) -o $@ $<

$(BUILD)/src/%.c: src/%.y | $(BUILD)/src
	$(YACC) -d -o $@ $<

$(BUILD)/test/%.o: test/%.cpp | $(BUILD)/test
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -I test -o $@ -c $<

$(BUILD)/src:
	mkdir -p $@

$(BUILD)/test:
	mkdir -p $@

$(BUILDINFO):
	@echo generate $(BUILDINFO)
	@echo "#define TSYM_COMMIT \"$(COMMIT)\"" >> $(BUILDINFO)
	@echo "#define TSYM_BRANCH \"$(BRANCH)\"" >> $(BUILDINFO)
	@if ! echo $(CXXFLAGS) | grep -qs 'DNDEBUG'; then echo "#define TSYM_DEBUG_STRINGS" >> $(BUILDINFO); fi
	@echo "#define TSYM_CPP_FLAGS \"$(CPPFLAGS)\"" >> $(BUILDINFO)
	@echo "#define TSYM_CXX_COMPILER \"`$(CXX) --version | head -n 1`\"" >> $(BUILDINFO)
	@echo "#define TSYM_CXX_FLAGS \"$(CXXFLAGS)\"" >> $(BUILDINFO)
	@echo "#define TSYM_C_COMPILER \"`$(CC) --version | head -n 1`\"" >> $(BUILDINFO)
	@echo "#define TSYM_C_FLAGS \"$(CFLAGS)\"" >> $(BUILDINFO)
	@echo "#define TSYM_PARSER_GENERATOR \"`$(YACC) --version | head -n 1`\"" >> $(BUILDINFO)
	@echo "#define TSYM_LEXICAL_ANALYZER \"`$(LEX) --version | head -n 1`\"" >> $(BUILDINFO)
	@echo "#define TSYM_BUILD_OS \"$(shell uname -mro)\"" >> $(BUILDINFO)
	@echo "#define TSYM_BUILD_DATE \"$(shell date +'%d. %b. %Y, %R %Z')\"" >> $(BUILDINFO)

$(TAGS): $(LIB_SRC) $(wildcard src/*.h) $(TEST_SRC)
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q -f $(TAGS) src/ test/

install: lib $(LIB_HEADER)
	install -m 644 -Dt $(DESTDIR)$(PREFIX)/include/$(NAME) $(LIB_HEADER) $(BUILDINFO) $(PUBLIC_HEADER:%=src/%.h)
	install -m 644 -Dt $(DESTDIR)$(PREFIX)/lib/ $(LIB_TARGET)

$(LIB_HEADER):
	@echo generate library header $(LIB_HEADER)
	@echo "#ifndef TSYM_TSYM_H" > $(LIB_HEADER)
	@echo -e "#define TSYM_TSYM_H\n" >> $(LIB_HEADER)
	@for header in $(PUBLIC_HEADER) buildinfo; do echo "#include $${header}.h" >> $(LIB_HEADER); done
	@echo -e "\n#endif TSYM_TSYM_H" >> $(LIB_HEADER)

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/lib/$(notdir $(LIB_TARGET))
	$(RM) -r $(DESTDIR)$(PREFIX)/include/$(NAME)

test: tests
	@$(TEST_EXEC)

clean:
	$(RM) $(LIB_OBJ) $(LIB_TARGET) $(LIB_PARSER_H) $(BUILDINFO)
	$(RM) $(TEST_OBJ) $(TEST_EXEC)
	$(RM) $(DEPS)
	$(RM) $(BUILD)/src/*.c
	$(RM) $(BUILD)/coverage/*

.PHONY: clean default lib tests test install uninstall
