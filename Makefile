CC = g++
CCFLAGS = -O0 -g
MAIN = main
MODULES = chtype lex synt
TARGET = r

all: $(TARGET)

doit:
	$(foreach var, $(MODULES), $(CC) $(CCFLAGS) -c $(var).cpp -o $(var).o;)

$(TARGET): $(MAIN).o $(foreach var, $(MODULES), $(var).o)
	$(CC) $(CCFLAGS) $(foreach var, $(MODULES), $(var).o) $(MAIN).o -o $(TARGET)

clear:
	rm -rf *.o r
