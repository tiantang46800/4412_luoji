// GPIO
#define GPA0CON (*(volatile unsigned int *)0x11400000)
#define GPA1CON (*(volatile unsigned int *)0x11400020)
// MPLL
#define MPLL_CON0 (*(volatile unsigned int *)0x10040108)
#define CLK_SRC_DMC (*(volatile unsigned int *)0x10040200)
#define CLK_SRC_TOP1 (*(volatile unsigned int *)0x1003C214)
// system clock
#define CLK_SRC_PERIL0 (*(volatile unsigned int *)0x1003C250)
#define CLK_DIV_PERIL0 (*(volatile unsigned int *)0x1003C550)
// UART
#define UFCON2 (*(volatile unsigned int *)0x13820008)
#define ULCON2 (*(volatile unsigned int *)0x13820000)
#define UCON2 (*(volatile unsigned int *)0x13820004)
#define UBRDIV2 (*(volatile unsigned int *)0x13820028)
#define UFRACVAL2 (*(volatile unsigned int *)0x1382002c)
#define UTXH2 (*(volatile unsigned int *)0x13820020)
#define URXH2 (*(volatile unsigned int *)0x13820024)

#define UTRSTAT2 (*(volatile unsigned int *)0x13820010)

void UartInit()
{
	/* 1.设置相应的GPIO用于串口功能 */
	unsigned long tmp = 0;

	tmp = GPA1CON;
	tmp &= ~(0xff); //设置UART0对应的GPIO为UART功能
	tmp |= 0x22;
	GPA1CON = tmp;

	//0x10040108
	//设置MPLL 输出 800MHz，FOUT = MDIV x FIN / (PDIV x 2 ^ SDIV) = 0x64 x 24MHz / (3 x 2^ 0) = 800MHz
	MPLL_CON0 = (1<<31 | 0x64<<16 | 0x3<<8 | 0x0);  //SDIV=0,PDIV=3,MDIV=100,enable=1

	//0x10040200
	CLK_SRC_DMC = 0x00011000; //bit[12]即MUX_MPLL_SEL=1, SCLKMPLLL使用MPLL的输出

	//0x1003c214
	CLK_SRC_TOP1 = 0x01111000; //bit[12]即MUX_MPLL_USER_SEL_T=1, MUX_MPLL_USER_SEL_T使用SCLKMPLLL

	//bit[3:0]即UART0_SEL=6, MOUTUART0使用SCLKMPLL_USER_T，所以, MOUTUART0即等于MPLL的输出, 800MHz
	CLK_SRC_PERIL0 = ((0 << 24) | (0 << 20) | (6 << 16) | (6 << 12) | (6<< 8) | (6 << 4) | (6)); 

	//bit[3:0]即UART0_RATIO=7,所以SCLK_UART0=MOUTUART0/(7+1)=100MHz
	CLK_DIV_PERIL0 = ((7 << 20) | (7 << 16) | (7 << 12) | (7 << 8) | (7 << 4) | (7)); 

	/* 3.设置串口0相关 */
	/* 设置FIFO中断触发阈值
	 * 使能FIFO
	 */
	UFCON2 = 0x111;

	/* 设置数据格式: 8n1, 即8个数据位,没有较验位,1个停止位 */
	ULCON2 = 0x3;

	/* 工作于中断/查询模式
	 * 另一种是DMA模式,本章不使用
	 */
	UCON2 = 0x5;

	/* SCLK_UART0=100MHz, 波特率设置为115200
	 * 寄存器的值如下计算:
	 * DIV_VAL = 100,000,000 / (115200 * 16) - 1 = 53.25
	 * UBRDIVn0 = 整数部分 = 53
	 * UFRACVAL0 = 小数部分 x 16 = 0.25 * 16 = 4
	 */
	UBRDIV2 = 53;
	UFRACVAL2 = 4;

}

char getc(void)
{
	char c;
	/* 查询状态寄存器，直到有有效数据 */
	while (!(UTRSTAT2 & (1<<0)));
	c = URXH2; /* 读取接收寄存器的值 */

	return c;
}

void putc(char c)
{
	/* 查询状态寄存器，直到发送缓存为空 */
	while (!(UTRSTAT2 & (1<<2)));

	UTXH2 = c; /* 写入发送寄存器 */

	return;
}

void puts(char *s)
{
	while (*s)
	{
		putc(*s);
		s++;
	}
}
int add (unsigned int a,unsigned int b)
{
	int sum;
	asm volatile 
	(
	 "add %0,%1,%2\n\t"
	 :"=r"(sum)
	 :"r"(a),"r"(b)
	 :"memory"
	 );
	return sum;
}
int main(void)
{
	char rec;

	UartInit();

	putc('n');
	putc('n');
	putc('n');
	putc('n');
	putc(add(65,0));//'A'
	while(1)
	{
		rec = getc();
		rec++;
		putc('\r');
		putc('\n');
		putc(rec);
	}

	return 0;
}
