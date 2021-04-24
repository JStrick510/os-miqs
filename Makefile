CC = gcc
CFLAGS = -pthread
LDFLAGS = -pthread
OBJFILES = code.o art.o linklist.o mem_perf.o
TARGET = codeexe

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
	
clean:
	rm -f $(OBJFILES) $(TARGET) *~
