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
// 关闭看门狗
ldr	r0, =0x10060000
mov	r1, #0
str	r1, [r0]



/*
* set GPX2_7 as output
*/
ldr r0, =0x11000c40 // GPX2CON的地址
ldr r1, [r0] // 先读出原值
bic r1, r1, #0xf0000000 // 清除bit[3:0]
orr r1, r1, #0x10000000 // 设置bit[3:0]为0b0001
str r1, [r0] // 写入GPM4CON


leds_loop:
/*
*set GPX2_7 as Low
*/
ldr r0, =0x11000c44 // GPX2DAT的地址
ldr r1, [r0] // 读出原值
mov r1,#0xff
str r1, [r0] // 写入


ldr r2,=0xffffff
bl delay


mov r1,#0
str r1, [r0] // 写1 led灭
ldr r2,=0xffffff
bl delay

b leds_loop

delay:
sub r2,r2,#1 //sub 减法
cmp r2,#0x0 //将r0 与0比较
bne delay //b是跳 ne 是不相等 ，不相等就跳到delay
mov pc,lr //lr 里存的是调用函数时的下一条指令，让Pc指针指向lr就可以完成函数的返回


halt_loop:
	b halt_loop

