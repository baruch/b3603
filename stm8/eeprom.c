#include "eeprom.h"
#include "stm8s.h"

static uint8_t eeprom_unlock_data(void)
{
	FLASH_DUKR = 0xAE; // Enable the flash data writing
	FLASH_DUKR = 0x56;

	return (FLASH_IAPSR & FLASH_IAPSR_DUL); // True if device unlocked
}

uint8_t eeprom_set_afr0(void)
{
	uint8_t sr;
	uint16_t timeout = 0xFFFF;

	if (!eeprom_unlock_data())
		return 0;

	FLASH_CR2 = FLASH_CR2_OPT;// Set the OPT bit
	FLASH_NCR2 = ~FLASH_NCR2_NOPT; // Remove the NOPT bit

	OPT2 = 1;
	NOPT2 = ~1;
	for (timeout = 0xFFFF; timeout > 0; timeout--) {
		sr = FLASH_IAPSR;
		if (sr & FLASH_IAPSR_EOP)
			break;
	}

	if (sr & FLASH_IAPSR_WR_PG_DIS)
		timeout = 0; // This will report failure

	FLASH_CR2 &= FLASH_CR2_OPT;
	FLASH_NCR2 |= FLASH_NCR2_NOPT;
	FLASH_IAPSR &= ~FLASH_IAPSR_DUL; // Disable write access
	return (timeout > 0) && (OPT2 & 1);
}
