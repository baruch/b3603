SRC=main.c display.c uart.c
OBJ=$(SRC:.c=.rel)
SDCC=sdcc -lstm8 -mstm8 --opt-code-size --std-c99

all: b3603.ihx

b3603.ihx: $(OBJ)
	$(SDCC) --out-fmt-ihx --code-size 8192 -o $@ $^

%.rel: %.c
	$(SDCC) -c -o $@ $<

clean:
	-rm -f *.rel *.ihx *.lk *.map *.rst *.lst *.asm *.sym

.PHONY: all clean
