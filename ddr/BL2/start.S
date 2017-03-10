.text
.globl _start
_start:

	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr,r0
	bl cpu_init_cp15
      // 关闭看门狗 
	/*
	 * for close watchdog
	 * PS-Hold high
	 */
#if 1
	ldr r0, =0x1002330c
	ldr r1, [r0]
	orr r1, r1, #0x300
	str r1, [r0]         
#endif
	ldr r0, =0x11000c08
	ldr r1, =0x0
	str r1, [r0]
	ldr r0, =0x1002040c
	ldr r1, =0x00
	str r1, [r0]

      ldr	r0, =0x10060000
      mov	r1, #0
      str	r1, [r0]
      
      // 打开icache可提高运行速度
      //mrc	p15, 0, r0, c1, c0, 0
      //orr	r0, r0, #0x00001000      //打开ICache
      //bic	r0, r0, #0x00001000      //关闭ICache
      //mcr	p15, 0, r0, c1, c0, 0
       
      ldr sp, =0x02027800 // 调用C函数之前必须设置栈,栈用于保存运行环境,给局部变量分配空间
                        // 参考ROM手册P14, 我们把栈指向BL2上方1K处(1K已经够用),
                        // 即:0x02020000 (iRAM基地址) + 5K(iROM代码用) + 8K(BL1用) + 16K(BL2用) + 1K(用作栈))
      bl system_clock_init // 调用时钟初始化函数
      
      bl uart_asm_init  /* init uart for debug */
      
      bl mem_init     //初始化DDR SDRAM
	//	bl test_uart
      bl copy_code_to_dram     //重定位，并跳到DRAM中运行
	

halt:
	b halt

cpu_init_cp15:
	/*
	 * Invalidate L1 I/D
	 */
	mov	r0, #0			@ set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	mcr	p15, 0, r0, c7, c5, 6	@ invalidate BP array
	mcr     p15, 0, r0, c7, c10, 4	@ DSB
	mcr     p15, 0, r0, c7, c5, 4	@ ISB

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00000002	@ set bit 1 (--A-) Align
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-cache
	mcr	p15, 0, r0, c1, c0, 0
	mov	pc, lr			@ back to my caller


