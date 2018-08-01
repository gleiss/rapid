SRCDIR=../src

PARSER_SRC=$(SRCDIR)/parser/WhileParser.cpp
PARSER_HDR=$(SRCDIR)/parser/WhileParser.hpp
SCANNER_SRC=$(SRCDIR)/parser/WhileScanner.cpp

BISON=/usr/local/opt/bison/bin/bison
FLEX=flex

all: $(PARSER_SRC) $(PARSER_HDR) $(SCANNER_SRC)

$(PARSER_SRC) $(PARSER_HDR): WhileParser.yy
	$(BISON) --defines=$(PARSER_HDR) -o $(PARSER_SRC) $^

$(SCANNER_SRC): WhileScanner.ll
	$(FLEX) -o $@ $^

.PHONY: clean mrproper

clean:

mrproper: 
	rm $(PARSER_SRC) $(PARSER_HDR) $(SCANNER_SRC) stack.hh
