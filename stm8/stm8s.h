/* GPIO */
#define PA_ODR *(unsigned char*)0x5000
#define PA_IDR *(unsigned char*)0x5001
#define PA_DDR *(unsigned char*)0x5002
#define PA_CR1 *(unsigned char*)0x5003
#define PA_CR2 *(unsigned char*)0x5004

#define PB_ODR *(unsigned char*)0x5005
#define PB_IDR *(unsigned char*)0x5006
#define PB_DDR *(unsigned char*)0x5007
#define PB_CR1 *(unsigned char*)0x5008
#define PB_CR2 *(unsigned char*)0x5009

#define PC_ODR *(unsigned char*)0x500A
#define PC_IDR *(unsigned char*)0x500B
#define PC_DDR *(unsigned char*)0x500C
#define PC_CR1 *(unsigned char*)0x500D
#define PC_CR2 *(unsigned char*)0x500E

#define PD_ODR *(unsigned char*)0x500F
#define PD_IDR *(unsigned char*)0x5010
#define PD_DDR *(unsigned char*)0x5011
#define PD_CR1 *(unsigned char*)0x5012
#define PD_CR2 *(unsigned char*)0x5013

#define PE_ODR *(unsigned char*)0x5014
#define PE_IDR *(unsigned char*)0x5015
#define PE_DDR *(unsigned char*)0x5016
#define PE_CR1 *(unsigned char*)0x5017
#define PE_CR2 *(unsigned char*)0x5018

#define PF_ODR *(unsigned char*)0x5019
#define PF_IDR *(unsigned char*)0x501A
#define PF_DDR *(unsigned char*)0x501B
#define PF_CR1 *(unsigned char*)0x501C
#define PF_CR2 *(unsigned char*)0x501D

/* CLOCK */
#define CLK_ICKR    *(unsigned char*)0x50C0
#define CLK_ECKR    *(unsigned char*)0x50C1
#define CLK_CMSR    *(unsigned char*)0x50C3
#define CLK_SWR     *(unsigned char*)0x50C4
#define CLK_SWCR    *(unsigned char*)0x50C5
#define CLK_CKDIVR  *(unsigned char*)0x50C6
#define CLK_PCKENR1 *(unsigned char*)0x50C7
#define CLK_CSSR    *(unsigned char*)0x50C8
#define CLK_CCOR    *(unsigned char*)0x50C9
#define CLK_PCKENR2 *(unsigned char*)0x50CA
#define CLK_HSITRIMR *(unsigned char*)0x50CC
#define CLK_SWIMCCR *(unsigned char*)0x50CD

/* ------------------- USART ------------------- */
#define USART1_SR *(unsigned char*)0x5230
#define USART1_DR *(unsigned char*)0x5231
#define USART1_BRR1 *(unsigned char*)0x5232
#define USART1_BRR2 *(unsigned char*)0x5233
#define USART1_CR1 *(unsigned char*)0x5234
#define USART1_CR2 *(unsigned char*)0x5235
#define USART1_CR3 *(unsigned char*)0x5236
#define USART1_CR4 *(unsigned char*)0x5237
#define USART1_CR5 *(unsigned char*)0x5238
#define USART1_GTR *(unsigned char*)0x5239
#define USART1_PSCR *(unsigned char*)0x523A

/* USART_CR1 bits */
#define USART_CR1_R8 (1 << 7)
#define USART_CR1_T8 (1 << 6)
#define USART_CR1_UARTD (1 << 5)
#define USART_CR1_M (1 << 4)
#define USART_CR1_WAKE (1 << 3)
#define USART_CR1_PCEN (1 << 2)
#define USART_CR1_PS (1 << 1)
#define USART_CR1_PIEN (1 << 0)

/* USART_CR2 bits */
#define USART_CR2_TIEN (1 << 7)
#define USART_CR2_TCIEN (1 << 6)
#define USART_CR2_RIEN (1 << 5)
#define USART_CR2_ILIEN (1 << 4)
#define USART_CR2_TEN (1 << 3)
#define USART_CR2_REN (1 << 2)
#define USART_CR2_RWU (1 << 1)
#define USART_CR2_SBK (1 << 0)

/* USART_CR3 bits */
#define USART_CR3_LINEN (1 << 6)
#define USART_CR3_STOP2 (1 << 5)
#define USART_CR3_STOP1 (1 << 4)
#define USART_CR3_CLKEN (1 << 3)
#define USART_CR3_CPOL (1 << 2)
#define USART_CR3_CPHA (1 << 1)
#define USART_CR3_LBCL (1 << 0)

/* USART_SR bits */
#define USART_SR_TXE (1 << 7)
#define USART_SR_TC (1 << 6)
#define USART_SR_RXNE (1 << 5)
#define USART_SR_IDLE (1 << 4)
#define USART_SR_OR (1 << 3)
#define USART_SR_NF (1 << 2)
#define USART_SR_FE (1 << 1)
#define USART_SR_PE (1 << 0)


/* ------------------- TIMERS ------------------- */
#define TIM1_CR1 *(unsigned char*)0x5250
#define TIM1_CR2 *(unsigned char*)0x5251
#define TIM1_SMCR *(unsigned char*)0x5252
#define TIM1_ETR *(unsigned char*)0x5253
#define TIM1_IER *(unsigned char*)0x5254
#define TIM1_SR1 *(unsigned char*)0x5255
#define TIM1_SR2 *(unsigned char*)0x5256
#define TIM1_EGR *(unsigned char*)0x5257
#define TIM1_CCMR1 *(unsigned char*)0x5258
#define TIM1_CCMR2 *(unsigned char*)0x5259
#define TIM1_CCMR3 *(unsigned char*)0x525A
#define TIM1_CCMR4 *(unsigned char*)0x525B
#define TIM1_CCER1 *(unsigned char*)0x525C
#define TIM1_CCER2 *(unsigned char*)0x525D
#define TIM1_CNTRH *(unsigned char*)0x525E
#define TIM1_CNTRL *(unsigned char*)0x525F
#define TIM1_PSCRH *(unsigned char*)0x5260
#define TIM1_PSCRL *(unsigned char*)0x5261
#define TIM1_ARRH *(unsigned char*)0x5262
#define TIM1_ARRL *(unsigned char*)0x5263
#define TIM1_RCR *(unsigned char*)0x5264
#define TIM1_CCR1H *(unsigned char*)0x5265
#define TIM1_CCR1L *(unsigned char*)0x5266
#define TIM1_CCR2H *(unsigned char*)0x5267
#define TIM1_CCR2L *(unsigned char*)0x5268
#define TIM1_CCR3H *(unsigned char*)0x5269
#define TIM1_CCR3L *(unsigned char*)0x526A
#define TIM1_CCR4H *(unsigned char*)0x526B
#define TIM1_CCR4L *(unsigned char*)0x526C
#define TIM1_BKR *(unsigned char*)0x526D
#define TIM1_DTR *(unsigned char*)0x526E
#define TIM1_OISR *(unsigned char*)0x526F

#define TIM2_CR1 *(unsigned char*)0x5300
#define TIM2_IER *(unsigned char*)0x5303
#define TIM2_SR1 *(unsigned char*)0x5304
#define TIM2_SR2 *(unsigned char*)0x5305
#define TIM2_EGR *(unsigned char*)0x5306
#define TIM2_CCMR1 *(unsigned char*)0x5307
#define TIM2_CCMR2 *(unsigned char*)0x5308
#define TIM2_CCMR3 *(unsigned char*)0x5309
#define TIM2_CCER1 *(unsigned char*)0x530A
#define TIM2_CCER2 *(unsigned char*)0x530B
#define TIM2_CNTRH *(unsigned char*)0x530C
#define TIM2_CNTRL *(unsigned char*)0x530D
#define TIM2_PSCR *(unsigned char*)0x530E
#define TIM2_ARRH *(unsigned char*)0x530F
#define TIM2_ARRL *(unsigned char*)0x5310
#define TIM2_CCR1H *(unsigned char*)0x5311
#define TIM2_CCR1L *(unsigned char*)0x5312
#define TIM2_CCR2H *(unsigned char*)0x5313
#define TIM2_CCR2L *(unsigned char*)0x5314
#define TIM2_CCR3H *(unsigned char*)0x5315
#define TIM2_CCR3L *(unsigned char*)0x5316

/* TIM_IER bits */
#define TIM_IER_BIE (1 << 7)
#define TIM_IER_TIE (1 << 6)
#define TIM_IER_COMIE (1 << 5)
#define TIM_IER_CC4IE (1 << 4)
#define TIM_IER_CC3IE (1 << 3)
#define TIM_IER_CC2IE (1 << 2)
#define TIM_IER_CC1IE (1 << 1)
#define TIM_IER_UIE (1 << 0)

/* TIM_CR1 bits */
#define TIM_CR1_APRE (1 << 7)
#define TIM_CR1_CMSH (1 << 6)
#define TIM_CR1_CMSL (1 << 5)
#define TIM_CR1_DIR (1 << 4)
#define TIM_CR1_OPM (1 << 3)
#define TIM_CR1_URS (1 << 2)
#define TIM_CR1_UDIS (1 << 1)
#define TIM_CR1_CEN (1 << 0)

/* TIM_SR1 bits */
#define TIM_SR1_BIF (1 << 7)
#define TIM_SR1_TIF (1 << 6)
#define TIM_SR1_COMIF (1 << 5)
#define TIM_SR1_CC4IF (1 << 4)
#define TIM_SR1_CC3IF (1 << 3)
#define TIM_SR1_CC2IF (1 << 2)
#define TIM_SR1_CC1IF (1 << 1)
#define TIM_SR1_UIF (1 << 0)


/* ------------------- ADC1 ------------------- */
#define ADC1_DB0H *(unsigned char*)0x53E0
#define ADC1_DB0L *(unsigned char*)0x53E1
#define ADC1_DB1H *(unsigned char*)0x53E2
#define ADC1_DB1L *(unsigned char*)0x53E3
#define ADC1_DB2H *(unsigned char*)0x53E4
#define ADC1_DB2L *(unsigned char*)0x53E5
#define ADC1_DB3H *(unsigned char*)0x53E6
#define ADC1_DB3L *(unsigned char*)0x53E7
#define ADC1_DB4H *(unsigned char*)0x53E8
#define ADC1_DB4L *(unsigned char*)0x53E9
#define ADC1_DB5H *(unsigned char*)0x53EA
#define ADC1_DB5L *(unsigned char*)0x53EB
#define ADC1_DB6H *(unsigned char*)0x53EC
#define ADC1_DB6L *(unsigned char*)0x53ED
#define ADC1_DB7H *(unsigned char*)0x53EE
#define ADC1_DB7L *(unsigned char*)0x53EF
#define ADC1_DB8H *(unsigned char*)0x53F0
#define ADC1_DB8L *(unsigned char*)0x53F1
#define ADC1_DB9H *(unsigned char*)0x53F2
#define ADC1_DB9L *(unsigned char*)0x53F3

#define ADC1_CSR *(unsigned char*)0x5400
#define ADC1_CR1 *(unsigned char*)0x5401
#define ADC1_CR2 *(unsigned char*)0x5402
#define ADC1_CR3 *(unsigned char*)0x5403
#define ADC1_DRH *(unsigned char*)0x5404
#define ADC1_DRL *(unsigned char*)0x5405
#define ADC1_TDRH *(unsigned char*)0x5406
#define ADC1_TDRL *(unsigned char*)0x5407
#define ADC1_HTRH *(unsigned char*)0x5408
#define ADC1_HTRL *(unsigned char*)0x5409
#define ADC1_LTRH *(unsigned char*)0x540A
#define ADC1_LTRL *(unsigned char*)0x540B
#define ADC1_AWSRH *(unsigned char*)0x540C
#define ADC1_AWSRL *(unsigned char*)0x540D
#define ADC1_AWCRH *(unsigned char*)0x540E
#define ADC1_AWCRL *(unsigned char*)0x540F

/* ---------------- CPU/SWIM registers ----------------*/
#define CFG_GCR *(unsigned char*)0x7F60
#define SWIM_CSR *(unsigned char*)0x7F80

/* IWDG */
#define IWDG_KR *(unsigned char *)0x50E0
#define IWDG_PR *(unsigned char *)0x50E1
#define IWDG_RLR *(unsigned char *)0x50E2

/* Option bytes */
#define OPT0 *(unsigned char *)0x4800
#define OPT1 *(unsigned char *)0x4801
#define NOPT1 *(unsigned char *)0x4802
#define OPT2 *(unsigned char *)0x4803
#define NOPT2 *(unsigned char *)0x4804
#define OPT3 *(unsigned char *)0x4805
#define NOPT3 *(unsigned char *)0x4806
#define OPT4 *(unsigned char *)0x4807
#define NOPT4 *(unsigned char *)0x4808
#define OPT5 *(unsigned char *)0x4809
#define NOPT5 *(unsigned char *)0x480A
