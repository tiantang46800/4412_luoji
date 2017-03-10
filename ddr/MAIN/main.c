
#define GPK2CON		(*(volatile unsigned int *)0x11000c40)
#define GPK2DAT		(*(volatile unsigned int *)0x11000c44)

void delay(volatile int time)
{
	for(; time > 0; time-- )
		;
}

int main(void)
{
	unsigned long tmp = 0;
	int i = 0;

	tmp = GPK2CON;
	tmp &= 0xf0000000;
	tmp |= 0x10000000;
	GPK2CON = tmp;
	
	/*
	 *  实现流水灯
	 */
	 while(1)
	 {

		GPK2DAT = 0x0;
  		delay(19999999);
		GPK2DAT = 0xff;
  		delay(19999999);
	 }
	

	return 0;
}
