CC := gcc
CFLAGS := -Wall -Wextra -g
SRCS := client.c helper.c requests.c parson.c
OBJS := $(SRCS:.c=.o)
BIN := client

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BIN)
