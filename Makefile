BIN      := maprender-test

TESTS    := tests/module
OBJ      := main.o graphics.o map.o mapview.o events.o module.o
SOBJ     := modules/gfx-sdl.so

SOURCES  := $(subst .o,.c,$(OBJ))
DEPFILES := $(subst .o,.d,$(OBJ))

CC       := clang
RM       := rm -f

MODPATH  := $(shell pwd)/modules/\0

WARN     := -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes

LIBS     := `sdl-config --libs` -lSDL_image -g
CFLAGS   := `sdl-config --cflags` -ansi -pedantic -O2 -ggdb \
            -DDEFMODPATH="\"$(MODPATH)\"" $(WARN)

.PHONY: all doc autodoc clean clean-tests clean-doc tests

all: $(BIN) doc autodoc

$(BIN): $(OBJ) $(SO)
	@$(CC) $(OBJ) -o "$(BIN)" $(LIBS) >/dev/null

-include $(DEPFILES)

clean: clean-tests clean-doc
	-@$(RM) $(BIN) *.o *.d *.so modules/*.so modules/*.o &>/dev/null

### Documentation
doc: doc/module.pdf doc/mapformat-internal.pdf

autodoc:
	@doxygen >/dev/null

clean-doc:
	-@$(RM) doc/*.{pdf,aux,log} &>/dev/null
	-@$(RM) -r autodoc

### Test Suite
tests: CFLAGS += -DTESTSUITE -DMODPATH="\"$(shell pwd)/tests/modules/\""
tests: $(TESTS)
	@for file in $(TESTS); do $$file &>/dev/null || echo "Test '$$file' failed."; done

tests/module: module.o tests/module.o tests/modules/test.so
	@$(CC) $(CFLAGS) module.o tests/module.o tests/modules/test.so -o $@ >/dev/null

clean-tests:
	-@$(RM) $(TESTS) tests/*.{o,so} tests/modules/*.{o,so} &>/dev/null

### File Types
%.o : %.c
	@$(CC) -c $< $(CFLAGS) -o $@ >/dev/null

%.d: %.c
	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) -f $@.$$$$

%.so : %.c
	@$(CC) $(CFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o) >/dev/null
	@$(CC) $(LIBS) -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o) >/dev/null

%.pdf: %.tex
	@pdflatex -output-directory=$(shell dirname $@) $^ >/dev/null
