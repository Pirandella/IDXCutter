CC := gcc
TARGET := IDXCutter
CFLAGS = -std=c99 -I $(IDIR) -Wall -g3

ODIR := ./obj
SDIR := ./src
IDIR := ./inc

_DEPS := idx.h
DEPS := $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ := main.o idx.o
OBJ := $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(TARGET)
