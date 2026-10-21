	PRESERVE8
	THUMB
		
StackTop	EQU		0x20018000

	AREA	RESET, DATA, READONLY
__Vectors
	DCD		StackTop
	DCD		Reset_Handler
	
	AREA 	MAIN, CODE, READONLY
	EXPORT  Reset_Handler
	EXPORT  LOOP
		
	ENTRY

Reset_Handler PROC
	;使能AHB1时钟的GPIOA位
	LDR 	R0, = 0x40023800
	LDR 	R1, = 0x01
	STR 	R1, [R0, #0x30]

	;配置GPIOA为推挽输出
	LDR		R0, = 0x40020000
	LDR		R1, [R0]
	BIC		R1, R1, #(0x03<<10)
	ORR 	R1, R1, #(0x01<<10)
	STR 	R1, [R0]
	;向PA5写入高电平

	ENDP
	
LOOP 
	LDR		R1, = (0x01<<5)
	STR		R1, [R0, #0x18]
	
	LDR		R2, = 0x100000
Delay1
	SUBS	R2, R2, #0x01
	BNE		Delay1
	
	LDR		R1, =(0x01<<(5+16))
	STR		R1, [R0, #0x18]
	
	LDR		R2, = 0x100000
Delay2
	SUBS	R2, R2, #0x01
	BNE		Delay2
	
	B LOOP
	
	END
	

	
	
	
	