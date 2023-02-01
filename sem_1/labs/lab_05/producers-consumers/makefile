CC := gcc
CFLAGS := -std=c99 # -Werror -Wall -Wpedantic -Wextra -Wvla
LINKFLAGS := -lpthread -lrt
INC_DIR := inc
SRC_DIR := src
OUT_DIR := out
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OUT_DIR)/%.o)
DEPENDS := $(SRCS:$(SRC_DIR)/%.c=$(OUT_DIR)/%.d)

_MAKE_OUT_DIR_ := $(shell mkdir -p $(OUT_DIR))

.PHONY: run clean

run: app.exe
	./app.exe

app.exe: $(OUT_DIR)/main.o $(OBJS)
	$(CC) $^ -o $@ $(LINKFLAGS)

src/%.o: src/%.c
	gcc -c $^ $(CFLAGS) -I $(HEADERS_DIR) -o $@

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -MMD -o $@ -c $<

-include $(DEPENDS)

clean:
	rm -f $(OUT_DIR)/* *.exe
