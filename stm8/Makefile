SRC=main.c display.c uart.c eeprom.c outputs.c config.c calibrate.c
CFLAGS= -lstm8 -mstm8 --opt-code-size --std-c99
OBJ=$(SRC:.c=.rel)
DEP=$(SRC:%.c=.%.d)
SDCC=sdcc $(CFLAGS)

all: b3603.ihx

-include $(DEP)

b3603.ihx: $(OBJ)
	@echo LINK $@
	@$(SDCC) --out-fmt-ihx --code-size 8192 -o $@ $^

.%.d: %.c
	@echo DEP $<
	@$(SDCC) -M $^ > $^.tmp.d
	@mv $^.tmp.d $@

%.rel: %.c .%.d
	@echo CC $<
	@$(SDCC) -c -o $@ $<

clean:
	-rm -f *.rel *.ihx *.lk *.map *.rst *.lst *.asm *.sym .*.d

.PHONY: all clean
