
include $(wildcard makefile.in Makefile.in)

BUILD ?= build
CXXFLAGS ?= -O2 -fPIC -pedantic -Wall -Wextra -Wconversion -Wsign-conversion -Wsign-compare -Wold-style-cast -DNDEBUG -std=c++14
CPPFLAGS += -I src
PROFILE ?=
LIBS ?=
TAGS ?= $(BUILD)/tags
SO ?= so
PREFIX ?= /usr/local

BUILDINFO = $(BUILD)/buildinfo.h
COMMIT = $(shell git log --pretty=format:%h -n 1 2> /dev/null || echo unknown)
BRANCH = $(shell git branch | grep '^\*' | cut -b3- || echo unknown)

NAME = tsym
TEST_EXEC = $(BUILD)/runtests
LIB_TARGET = $(BUILD)/lib$(NAME).$(SO)
LIB_STATIC = $(BUILD)/lib$(NAME).a
LIB_HEADER = $(BUILD)/$(NAME).h
PUBLIC_HEADER = globals logger plu var version

LIB_SRC = $(wildcard src/*.cpp)
LIB_OBJ = $(LIB_SRC:%.cpp=$(BUILD)/%.o)
TEST_SRC = $(wildcard test/*.cpp)
TEST_OBJ = $(TEST_SRC:%.cpp=$(BUILD)/%.o)
DEPS = $(LIB_OBJ:%.o=%.d) $(TEST_OBJ:%.o=%.d)

default: tests
tests: $(TEST_EXEC)
lib: $(LIB_TARGET)
staticlib: $(LIB_STATIC)
tags: $(TAGS)

$(TEST_EXEC): $(LIB_OBJ) $(TEST_OBJ)
	$(CXX) -o $(TEST_EXEC) $(PROFILE) $(LIB_OBJ) $(TEST_OBJ) $(LDFLAGS) $(LIBS) -lCppUTest

$(LIB_TARGET): $(LIB_OBJ) $(BUILDINFO)
	$(CXX) -shared -o $(LIB_TARGET) $(PROFILE) $(LIB_OBJ) $(LIBS)

$(LIB_STATIC): $(LIB_OBJ) $(BUILDINFO)
	$(AR) rcs $@ $(LIB_OBJ)

-include $(DEPS)

$(BUILD)/src/%.o: src/%.cpp | $(BUILD)/src
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(PROFILE) -o $@ -c $<

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
	@echo "#define TSYM_CPP_FLAGS \"$(CPPFLAGS)\"" >> $(BUILDINFO)
	@echo "#define TSYM_CXX_COMPILER \"`$(CXX) --version | head -n 1`\"" >> $(BUILDINFO)
	@echo "#define TSYM_CXX_FLAGS \"$(CXXFLAGS)\"" >> $(BUILDINFO)
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
	@for header in $(PUBLIC_HEADER) buildinfo; do echo "#include \"$${header}.h\"" >> $(LIB_HEADER); done
	@echo -e "\n#endif" >> $(LIB_HEADER)

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/lib/$(notdir $(LIB_TARGET))
	$(RM) -r $(DESTDIR)$(PREFIX)/include/$(NAME)

test: tests
	@$(TEST_EXEC)

clean:
	$(RM) $(LIB_OBJ) $(LIB_TARGET) $(LIB_STATIC) $(BUILDINFO)
	$(RM) $(TEST_OBJ) $(TEST_EXEC)
	$(RM) $(DEPS)
	$(RM) $(BUILD)/src/*.c

.PHONY: clean default lib staticlib tags tests test install uninstall
