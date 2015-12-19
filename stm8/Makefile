SRC=main.c display.c uart.c eeprom.c outputs.c config.c fixedpoint.c parse.c adc.c
CFLAGS= -lstm8 -mstm8 --opt-code-size --std-c99
OBJ=$(SRC:.c=.rel)
DEP=$(SRC:%.c=.%.c.d)

V=0

ACTUAL_SDCC=sdcc $(CFLAGS)
SDCC_0 = @echo SDCC $<; $(ACTUAL_SDCC)
SDCC_1 = $(ACTUAL_SDCC)
SDCC = $(SDCC_$(V))

LINK_0 = @echo LINK $@; $(ACTUAL_SDCC)
LINK_1 = $(ACTUAL_SDCC)
LINK = $(LINK_$(V))

all: b3603.ihx check_size

-include $(DEP)

check_size: b3603.ihx
		@CODESIZE=$$(grep '^CODE' b3603.map |head -n1 | sed -e 's/^.*=\s*\([0-9]\+\).*$$/\1/'); \
			 if [ "$$CODESIZE" -gt 8192 ]; then echo "Code is too large, it is $$CODESIZE bytes"; exit 1; \
			 else echo "Code fits the flash, it is $$CODESIZE"; \
			 fi

b3603.ihx: $(OBJ)
	$(LINK) --out-fmt-ihx --code-size 8192 -o $@ $^

.%.c.d: %.c
	@$(ACTUAL_SDCC) -M -o $@ $<

%.rel: %.c
	$(SDCC) -c -o $@ $<

test_pwm_accuracy: test_pwm_accuracy.c outputs.c config.c fixedpoint.c
	gcc -g -Wall -o $@ $< -DTEST=1

test_adc_accuracy: test_adc_accuracy.c config.c adc.c fixedpoint.c
	gcc -g -Wall -o $@ $< -DTEST=1

test_parse: test_parse.c parse.c
	gcc -g -Wall -o $@ $< -DTEST=1

clean:
	-rm -f *.rel *.ihx *.lk *.map *.rst *.lst *.asm *.sym .*.d

.PHONY: all clean check_size
