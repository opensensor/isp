CC = mipsel-linux-gnu-gcc
CFLAGS = -Wall -O2
LDFLAGS =

SRC = src/isp.c src/sensor.c
OBJ = $(SRC:.c=.o)

TARGET = libisp.a

all: $(TARGET)

$(TARGET): $(OBJ)
    ar rcs $@ $^

%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

clean:
    rm -f $(OBJ) $(TARGET)

.PHONY: all clean