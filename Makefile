CC = g++
CCFLAGS = -O0 -g
MAIN = main
MODULS = lex synt
TARGET = r

all: $(TARGET)

doit:
	$(foreach var, $(MODULS), $(CC) $(CCFLAGS) -c $(var).cpp -o $(var).o;)

$(TARGET): $(MAIN).o $(foreach var, $(MODULS), $(var).o)
	$(CC) $(CCFLAGS) $(foreach var, $(MODULS), $(var).o) $(MAIN).o -o $(TARGET)

clear:
	rm -rf *.o r
