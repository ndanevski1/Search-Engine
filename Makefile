CC = g++ `~/.local/bin/xapian-config --cxxflags --libs`
OBJ = metadata_indexer metadata_search keyword_search
OBJS = $(OBJDIR)/common.o
OBJDIR = obj
SRCDIR = src

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	mkdir -p obj; $(CC) -o $@ -c $<

all: test_xapian $(OBJ)

metadata_indexer: $(OBJDIR)/metadata_indexer.o $(OBJS)
	$(CC) -o $@ $^

metadata_search: $(OBJDIR)/metadata_search.o $(OBJS)
	$(CC) -o $@ $^

keyword_search: $(OBJDIR)/keyword_search.o $(OBJS)
	$(CC) -o $@ $^

test_xapian:
	test -s ~/.local/bin/xapian-config || { echo "xapian-config does not exist! Aborting..."; exit 1; }

clean:
	rm -r -f $(OBJ) obj index

