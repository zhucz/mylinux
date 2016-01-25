#define GPACON 		(*(volatile unsigned int *)0x56000000)
#define GPADAT 		(*(volatile unsigned int *)0x56000004)



#define GPBCON 		(*(volatile unsigned int *)0x56000010)
#define GPBDAT 		(*(volatile unsigned int *)0x56000014)
#define GPBUDP 		(*(volatile unsigned int *)0x56000018)
#define GPBSEL 		(*(volatile unsigned int *)0x5600001c)


#define rGPHCON		(*(volatile unsigned *)0x56000070)
#define rGPHUDP		(*(volatile unsigned *)0x50000078)

#define rULCON0 	(*(volatile unsigned *)0x50000000)
#define rUCON0 		(*(volatile unsigned *)0x50000004)
#define rUFCON0 	(*(volatile unsigned *)0x50000008)
#define rUMCON0 	(*(volatile unsigned *)0x5000000c)
#define rUTRSTAT0 	(*(volatile unsigned *)0x50000010)
#define rUERSTAT0 	(*(volatile unsigned *)0x50000014)
#define rUFSTAT0 	(*(volatile unsigned *)0x50000018)
#define rUMSTAT0	(*(volatile unsigned *)0x5000001c)
#define rUBRDIV0 	(*(volatile unsigned *)0x50000028)
#define rUDIVSLOT0 	(*(volatile unsigned *)0x5000002c)

#define WrUTXH0(ch)	(*(volatile unsigned char *)0x50000020) = (unsigned char)(ch)

#define RUNLED  	0
#define PIDLED  	1
#define BEEP  		2
#define ALARMLED  	3

#define BEEP_DURATION	100000000	

#define DM9000_outb(d,r) 	( *(volatile unsigned char *)r = d )
#define DM9000_outw(d,r) 	( *(volatile unsigned int *)r = d )
#define DM9000_outl(d,r) 	( *(volatile unsigned long *)r = d )
#define DM9000_inb(r) 		( *(volatile unsigned char *)r)
#define DM9000_inw(r) 		( *(volatile unsigned int *)r)
#define DM9000_inl(r) 		( *(volatile unsigned long *)r)


#if 0
#define CONFIG_DRIVER_DM9000 			1
#define CONFIG_DM9000_BASE 				0x08000004
#define DM9000_IO 						CONFIG_DM9000_BASE
#define DM9000_DATA 					(CONFIG_DM9000_BASE + 4)
#define CONFIG_DM9000_USE_16BIT
#else
#define CONFIG_DRIVER_DM9000 			1
#define CONFIG_DM9000_BASE 				0x08000300
#define DM9000_IO 						CONFIG_DM9000_BASE
#define DM9000_DATA 					(CONFIG_DM9000_BASE + 8)
#define CONFIG_DM9000_USE_16BIT

#endif


#define DM9000_GPCR			   0x1e
#define DM9000_GPR             0x1f
#define DM9000_NCR             0x00
#define DM9000_PIDL            0x2A
#define DM9000_PIDH            0x2B
#define GPCR_GPIO0_OUT		   (1<<0)
#define NCR_LBK_INT_MAC		   (1<<1)
#define NCR_RST			       (1<<0)


#define SMBCR1_REG	(*(volatile unsigned long *)0x4F000034)


#if 1
static inline void delay(unsigned long loops)
{
	__asm__ volatile (
			"1: \n"
			"subs %0, %1, #1\n"
			"bne 1b":"=r"(loops):"0"(loops));
}
#else
void delay(unsigned int nCount)
{
	__asm__ volatile(
		"Delay1:\n\t"
		"LDR R1,=100000\n\t"
		"Delay2:\n\t"
		"SUBS R1,R1,#1\n\t"
		"BNE Delay2\n\t"
		"SUBS R0,R0,#1\n\t"
		"BNE Delay1\n\t"
		"BX LR\n\t"			
	);
}
#endif

void Gpio_Init(void)
{
//	GPBCON |= (1 << 6) | (1 << 18);
//	GPBUDP |= (2 << 6) | (2 << 18);
//	GPBDAT |= (1 << 3) | (1 << 9);
//	GPBDAT &= ~((1 << 3)|(1 << 9));


//uart0 
	rGPHCON |= (2 << 0) | (2 << 2);
	rGPHUDP |= (2 << 0) | (2 << 2);
//DM9000 
 	GPACON	|= (1 << 12);//NRCS[1] BANK1 08000000
}


void Uart0_Init(void)
{
	rULCON0 = 0x03;
	rUCON0 = 0x245;
	rUFCON0 = 0x00;
	rUMCON0 = 0x00;
	rUBRDIV0 = 34;
	rUDIVSLOT0 = 0xdddd;
	
}


/*
   Read a byte from I/O port
*/
static unsigned char  DM9000_ior(int reg)
{
	DM9000_outb(reg, DM9000_IO);
	return DM9000_inb(DM9000_DATA);
}

/*
   Write a byte to I/O port
*/
static void DM9000_iow(int reg, unsigned char value)
{
	DM9000_outb(reg, DM9000_IO);
	DM9000_outb(value, DM9000_DATA);
}

/* General Purpose dm9000 reset routine */
static void	dm9000_reset(void)
{

	/* Reset DM9000,
	   see DM9000 Application Notes V1.22 Jun 11, 2004 page 29 */

	/* DEBUG: Make all GPIO0 outputs, all others inputs */
	DM9000_iow(DM9000_GPCR, GPCR_GPIO0_OUT);
	/* Step 1: Power internal PHY by writing 0 to GPIO0 pin */
	DM9000_iow(DM9000_GPR, 0);
	/* Step 2: Software reset */
	DM9000_iow(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST));

	do {
//		DM9000_DBG("resetting the DM9000, 1st reset\n");
		delay(25000000); /* Wait at least 20 us */
		WrUTXH0(0x55);
	} while (DM9000_ior(DM9000_NCR) & 1);

	DM9000_iow(DM9000_NCR, 0);
	DM9000_iow(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST)); /* Issue a second reset */

	do {
//		DM9000_DBG("resetting the DM9000, 2nd reset\n");
		delay(25000000); /* Wait at least 20 us */
		WrUTXH0(0x66);
	} while (DM9000_ior(DM9000_NCR) & 1);

	/* Check whether the ethernet controller is present */
	if ((DM9000_ior(DM9000_PIDL) != 0x0) || (DM9000_ior(DM9000_PIDH) != 0x90)){
		WrUTXH0(DM9000_ior(DM9000_PIDL));
		delay(BEEP_DURATION);
		WrUTXH0(DM9000_ior(DM9000_PIDH));
		delay(BEEP_DURATION);
	}
}


int main(void)
{
	Gpio_Init();	
	Uart0_Init();

	SMBCR1_REG = 0x303010;

	dm9000_reset();

	while(1)
	{
		GPBDAT |= (1 << 3) | (1 << 9);
		delay(BEEP_DURATION);
		GPBDAT &= ~((1 << 3)|(1 << 9));
		delay(BEEP_DURATION);
	}
}
