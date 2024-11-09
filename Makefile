CC = $(shell echo ~/output/cinnado_d1_t31l/host/bin/mipsel-linux-gcc)
CFLAGS = -Wall -O2 -Iinclude
SRC = src/isp_init.c
OBJ = src/isp_init.o
TARGET = isp_init

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

src/isp_init.o: $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)


