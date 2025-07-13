CC = gcc
CFLAGS = -Wall -pthread
SRC = src/xor_encrypt.c
TARGET = xor_encrypt

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
