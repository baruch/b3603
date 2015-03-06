SRC=main.c display.c uart.c eeprom.c outputs.c config.c calibrate.c fixedpoint.c parse.c adc.c
CFLAGS= -lstm8 -mstm8 --opt-code-size --std-c99
OBJ=$(SRC:.c=.rel)
DEP=$(SRC:%.c=.%.c.d)
SDCC=sdcc $(CFLAGS)

all: b3603.ihx check_size

-include $(DEP)

check_size: b3603.ihx
		@CODESIZE=$$(grep '^CODE' b3603.map |head -n1 | sed -e 's/^.*=\s*\([0-9]\+\).*$$/\1/'); \
			 if [ "$$CODESIZE" -gt 8192 ]; then echo "Code is too large, it is $$CODESIZE bytes"; exit 1; \
			 else echo "Code fits the flash, it is $$CODESIZE"; \
			 fi

b3603.ihx: $(OBJ)
	@echo LINK $@
	@$(SDCC) --out-fmt-ihx --code-size 8192 -o $@ $^

%.rel: %.c
	@echo CC $<
	@$(SDCC) -M $< > .$<.tmp.d
	@mv .$<.tmp.d .$<.d
	@$(SDCC) -c -o $@ $<

test_pwm_accuracy: test_pwm_accuracy.c parse.c outputs.c config.c fixedpoint.c
	gcc -g -Wall -o $@ $< -DTEST=1

test_adc_accuracy: test_adc_accuracy.c config.c
	gcc -g -Wall -o $@ $< -DTEST=1

test_parse: test_parse.c parse.c
	gcc -g -Wall -o $@ $< -DTEST=1

clean:
	-rm -f *.rel *.ihx *.lk *.map *.rst *.lst *.asm *.sym .*.d

.PHONY: all clean check_size
