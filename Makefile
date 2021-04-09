# Makefile
CC = g++
OBJ = metadata_indexer metadata_search keyword_search

OBJS = $(OBJDIR)/common.o $(OBJDIR)/metadata_indexer.o $(OBJDIR)/metadata_search.o $(OBJDIR)/keyword_search.o
OBJDIR = obj
SRCDIR = src

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	mkdir -p obj; $(CC) -o $@ -c $<

metadata_indexer: $(OBJDIR)/metadata_indexer.o $(OBJS)
	$(CC) -o $@ $^

metadata_search: $(OBJDIR)/metadata_search.o $(OBJS)
	$(CC) -o $@ $^

keyword_search: $(OBJDIR)/keyword_search.o $(OBJS)
	$(CC) -o $@ $^

all: $(OBJ)

clean:
	rm -r -f $(OBJ) obj index