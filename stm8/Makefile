SRC=main.c
OBJ=$(SRC:.c=.rel)
SDCC=sdcc -lstm8 -mstm8 --opt-code-size

all: b3603.ihx

b3603.ihx: $(OBJ)
	$(SDCC) --out-fmt-ihx -o $@ $<

%.rel: %.c
	$(SDCC) -c -o $@ $<

clean:
	-rm -f *.rel *.ihx *.lk *.map *.rst *.lst *.asm *.sym

.PHONY: all clean
