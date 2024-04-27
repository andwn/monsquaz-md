#include "md.h"

/* poor man's debug - set bg color */
.macro SETBG _color
		move.l	#CRAM_ADDR_CMD,(VDP_CTRL_PORT)
		move.w	#\_color,(VDP_DATA_PORT)
.endm

#define CONSOLE_VER	0xA10001
#define TMSS		0xA14000

.section .bss

v_err_reg:	ds.l	16
v_err_pc:	ds.l	1
v_err_addr:	ds.l	1
v_err_inst:	ds.w	1
v_err_sr:	ds.w	1
v_err_type:	ds.b	1

	.global v_vbl_flag
v_vbl_flag:	ds.b	1

.section .text.keepboot

		dc.l	__stack,_start
		dc.l	_bus_error,_address_error,_illegal_inst,_zero_divide
		.space	22*4
		dc.l	_hblank,0,_vblank,0
		.space	32*4

rom_header:
		.ascii	"SEGA MEGA DRIVE "
		.ascii	"SKYCHASE 2024.04"
		.ascii	"MONSQUAZ                "
		.ascii	"                        "
		.ascii	"MONSQUAZ                "
		.ascii	"                        "
		.ascii	"GM ANDYG007-00"
		dc.w	0
		.ascii	"J6              "
		dc.l	__text_start
		dc.l	__text_size-1
		dc.l	0xFF0000
		dc.l	0xFFFFFF
		.ascii	"  "
		dc.w	0x2020
		dc.l	0x20202020
		dc.l	0x20202020
		.ascii	"            "
		.ascii	"                                        "
		.ascii	"JUE             "

	.global blank_data
blank_data:
		.space	0x100

_start:
		move	#0x2700,sr
		move.b	(CONSOLE_VER),d0		/* Check console version */
		andi.b	#0x0F,d0				/* Version 0 = skip TMSS */
		beq.s	skip_tmss
		move.l	(rom_header),(TMSS)		/* Write 'SEGA' to TMSS register */
	skip_tmss:
		/* Shut up PSG */
		lea		(0xC00011),a0
		move.b	#0x9F,d0				/* Channel 0 att 15 */
		move.b	#0x20,d1				/* Channel offset */
		move.b	d0,(a0)
		add.b	d1,d0
		move.b	d0,(a0)
		add.b	d1,d0
		move.b	d0,(a0)
		add.b	d1,d0
		move.b	d0,(a0)

		lea		__text_end,a0			/* Start of .data segment init values in ROM */
		lea		__data_start,a1			/* Start of .data segment in RAM */
		move.w	#__data_size,d0			/* Size of .data segment */
		lsr.w	#1,d0
		beq		2f
		subq.w	#1,d0					/* sub extra iteration */
	1:
		move.w	(a0)+,(a1)+				/* Copy initialized data to RAM */
		dbra	d0,1b
	2:
		move.w	#__bss_size,d0			/* Size of .bss segment in RAM */
		lsr.w	#1,d0
		beq		4f
		moveq	#0,d1
		subq.w	#1,d0					/* sub extra iteration */
	3:
		move.w	d1,(a1)+				/* Copy zero data to RAM */
		dbra	d0,3b
	4:

init_z80:
		move.w	#0x000,(Z80_RESET)
		move.w	#0x100,(Z80_BUS_REQ)
		move.w	#0x100,(Z80_RESET)

		lea		z80_program(pc),a0
		lea		Z80_RAM,a1
		move.w	#(z80_program_end-z80_program)-1,d1
	5:
		move.b	(a0)+,(a1)+				/* needs to be 1 byte at a time */
		dbf		d1,5b

		lea		(Z80_DRV_PARAMS+0x20),a1
		move.l	#blank_data,d0
		lsr.w	#8,d0
		move.b	d0,(a1)+
		swap	d0
		move.b	d0,(a1)+
		move.b	#1,(a1)+
		move.b	#0,(a1)

		move.w	#0x000,(Z80_RESET)
		move.w	#20,d0
	6:
		dbf		d0,6b
		move.w	#0x100,(Z80_RESET)
		move.w	#0x000,(Z80_BUS_REQ)

init_vdp:
		lea		(VDP_CTRL_PORT),a0
		lea		(VDP_DATA_PORT),a1
		move.w	#0x8004,(a0)
		move.w	#0x8134,(a0)		;// Disable display
		move.w	#0x8230,(a0)		;// Plane A 0xC000
		move.w	#0x832C,(a0)		;// Window  0xB000
		move.w	#0x8407,(a0)		;// Plane B 0xE000
		move.w	#0x857C,(a0)		;// Sprites 0xF800
		move.w	#0x8600,(a0)
		move.w	#0x8700,(a0)		;// Background color palette index
		move.w	#0x8800,(a0)
		move.w	#0x8900,(a0)
		move.w	#0x8A01,(a0)		;// Horizontal interrupt timer
		move.w	#0x8B00,(a0)		;// Scroll mode
		move.w	#0x8C81,(a0)		;// No interlace or shadow/highlight
		move.w	#0x8D3F,(a0)		;// HScroll 0xFC00
		move.w	#0x8E00,(a0)
		move.w	#0x8F02,(a0)		;// Auto increment
		move.w	#0x9001,(a0)		;// Map size (64x32)
		move.w	#0x9100,(a0)		;// Window X
		move.w	#0x9200,(a0)		;// Window Y

		moveq	#0,d0
		move.l	#0x40000000,(a0)
		move.w	#0x3FFF,d1
	7:
		move.l	d0,(a1)
		dbf		d1,7b

		move.w	#0x1F,d1
		move.l	#0xC0000000,(a0)
	8:
		move.l	d0,(a1)
		dbf		d1,8b

		move.l	#0x40000010,(a0)
		move.l	d0,(a1)

		jmp		main


z80_program:
	.incbin "src/z80_pcm.bin"
z80_program_end:
	.even

	.global pcm_start
pcm_start:
		PAUSE_Z80
		lea		Z80_DRV_PARAMS,a0
		move.l	#pcm_data,d0
		move.l	#pcm_data_end,d1
		sub.l	d0,d1
		lsr.w	#8,d0
		move.b	d0,(a0)+
		swap	d0
		move.b	d0,(a0)+
		lsr.w	#8,d1
		move.b	d1,(a0)+
		swap	d1
		move.b	d1,(a0)+
		move.b	#1,(a0)+				/* 22050 Hz */
		move.b	#0,(a0)+				/* unused */
		move.b	#0xC0,(a0)				/* both speakers */
		move.b	#1,(Z80_DRV_COMMAND)
		RESUME_Z80
		rts

_vblank:
		st.b	(v_vbl_flag)
_hblank:
		rte


_illegal_inst:
		move.b	#1,(v_err_type)
		bra.s	5f
_zero_divide:
		move.b	#2,(v_err_type)
		bra.s	6f
_bus_error:
_address_error:
		move.b	#0,(v_err_type)
		move.l	2(sp),(v_err_addr)
		move.w	6(sp),(v_err_inst)
		move.w	8(sp),(v_err_sr)
		move.l	10(sp),(v_err_pc)
		bra.s	8f
	5:
		move.b	#1,(v_err_type)
		bra.s	7f
	6:
		move.b	#2,(v_err_type)
	7:
		move.w	(sp),(v_err_sr)
		move.l	2(sp),(v_err_pc)
	8:
		move.l	a6,(v_err_reg+12)
		lea		(v_err_reg),a6
		move.l	d7,(a6)+
		move.l	a7,(a6)+
		move.l	d6,(a6)+
		adda.w	#4,a6
		move.l	d5,(a6)+
		move.l	a5,(a6)+
		move.l	d4,(a6)+
		move.l	a4,(a6)+
		move.l	d3,(a6)+
		move.l	a3,(a6)+
		move.l	d2,(a6)+
		move.l	a2,(a6)+
		move.l	d1,(a6)+
		move.l	a1,(a6)+
		move.l	d0,(a6)+
		move.l	a0,(a6)+

print_error:
		lea		(VDP_CTRL_PORT).l,a5
		lea		(VDP_DATA_PORT).l,a6
		move.w	#0x8B00,(a5)		/* Plane scroll mode */
		move.w	#0x8F02,(a5)		/* Auto-increment = 2 */
		move.l	#0x7F000000,(a5)	/* VRAM 0xFC00 */
		move.w	#0,(a6)				/* Plane A H scroll = 0 */
		move.l	#0x40000010,(a5)	/* VSRAM 0x0000 */
		move.w	#0,(a6)				/* Plane A V scroll = 0 */
		move.l	#0x7E000000,(a5)	/* VRAM 0xF800 */
		move.l	#0,(a6)				/* Clear sprite table */
		move.l	#0,(a6)
		move.w	#0x9100,(a5)		/* Hide window plane */
		move.w	#0x9200,(a5)

		lea		(ErrFont).l,a0
		move.l	#0x40000000,(a5)	/* VRAM 0x0000 */
		moveq.l	#0,d5
		moveq.l	#30,d2
	0:
		move.l	d5,(a6)
		moveq.l	#2,d3
	1:
		move.b	(a0)+,d0
		moveq.l	#1,d4
	2:
		moveq.l	#0,d1
		btst.b	#6,d0
		sne.b	d1
		lsl.l	#8,d1
		btst.b	#5,d0
		sne.b	d1
		lsl.l	#8,d1
		btst.b	#4,d0
		sne.b	d1
		btst.b	#7,d0
		beq.s	3f
		andi.l	#0xFF0FFFFF,d1
		lsl.l	#4,d1
	3:
		lsl.l	#4,d1
		move.l	d1,(a6)

		lsl.b	#4,d0
		dbf		d4,2b
		dbf		d3,1b
		move.l	d5,(a6)
		dbf		d2,0b

		move.b	(v_err_type).l,d5
		lsl.w	#4,d5
		lea		StrError(pc, d5.w),a0

		move.l	#0x40000003+(0x82<<16),(a5)	/* VRAM 0xA082 */
		move.w	#0x8000,d0		/* Clear with priority bit set */
		moveq.l	#15,d4
	4:
		move.b	(a0)+,d0
		addq.b	#1,d0
		move.w	d0,(a6)
		dbf		d4,4b
		bra.s	DrawRegs

StrError:	dc.b	10,13,13,22,14,23,23,255,14,22,22,20,22,255,255,255
			dc.b	17,18,18,14,16,10,18,255,17,19,23,24,22,25,12,24
			dc.b	27,14,22,20,255,13,17,26,17,13,14,255,255,255,255,255

DrawRegs:
		lea		(v_err_reg),a0
		move.l	#0x40000003+(0x502<<16),d6	/* VRAM 0xA502 */
		move.w	#7,d5
	5:
		move.l	d6,(a5)		/* Set pos in Plane A */
		move.b	#14,d0		/* D */
		move.w	d0,(a6)
		move.b	d5,d0		/* # */
		addq.b	#1,d0
		move.w	d0,(a6)
		move.b	#29,d0		/* = */
		move.w	d0,(a6)
		move.l	(a0)+,d1
		bsr		PrintHex4
		move.b	#0,d0		/* _ */
		move.w	d0,(a6)
		move.b	#11,d0		/* A */
		move.w	d0,(a6)
		move.b	d5,d0		/* # */
		addq.b	#1,d0
		move.w	d0,(a6)
		move.b	#29,d0		/* = */
		move.w	d0,(a6)
		move.l	(a0)+,d1
		bsr		PrintHex4

		sub.l	#0x00800000,d6	/* Next row in nametable */
		dbf		d5,5b

		move.l	(v_err_reg+4),a0
		move.l	#0x40000003+(0xD32<<16),d6
		move.w	#23,d5
	6:
		move.l	d6,(a5)

		move.b	#24,d0	/* S */
		move.w	d0,(a6)
		move.b	#22,d0	/* P */
		move.w	d0,(a6)
		move.b	#30,d0	/* + */
		move.w	d0,(a6)
		move.b	d5,d1	/* # */
		add.b	d1,d1
		add.b	d1,d1
		bsr		PrintHex1
		move.b	#29,d0	/* = */
		move.w	d0,(a6)
		move.l	(a0)+,d1
		bsr		PrintHex4

		sub.l	#0x00800000,d6
		dbf		d5,6b

		move.l	#0x40000003+(0x602<<16),(a5)
		move.b	#22,d0	/* P */
		move.w	d0,(a6)
		move.b	#13,d0	/* C */
		move.w	d0,(a6)
		move.b	#29,d0	/* = */
		move.w	d0,(a6)
		move.l	(v_err_pc),d1
		bsr		PrintHex3

		move.l	#0x40000003+(0x682<<16),(a5)
		move.b	#24,d0	/* S */
		move.w	d0,(a6)
		move.b	#23,d0	/* R */
		move.w	d0,(a6)
		move.b	#29,d0	/* = */
		move.w	d0,(a6)
		move.w	(v_err_sr),d1
		bsr		PrintHex2

		cmp.b	#0,(v_err_type)		/* No more params */
		bne.s	7f

		move.l	#0x40000003+(0x782<<16),(a5)
		move.b	#21,d0	/* O */
		move.w	d0,(a6)
		move.b	#22,d0	/* P */
		move.w	d0,(a6)
		move.b	#29,d0	/* = */
		move.w	d0,(a6)
		move.w	(v_err_inst),d1
		bsr		PrintHex2

		move.l	#0x40000003+(0x802<<16),(a5)
		move.b	#11,d0	/* A */
		move.w	d0,(a6)
		move.b	#14,d0	/* D */
		move.w	d0,(a6)
		move.b	#29,d0	/* = */
		move.w	d0,(a6)
		move.l	(v_err_addr),d1
		bsr		PrintHex3

	7:
		bra.s	7b

/* d1 = integer */
/* breaks d0.b */
PrintHex4:
		moveq.l	#7,d2
		bra.s	PrintHexLoop
PrintHex3:
		rol.l	#8,d1
		moveq.l	#5,d2
		bra.s	PrintHexLoop
PrintHex2:
		swap	d1
		moveq.l	#3,d2
		bra.s	PrintHexLoop
PrintHex1:
		ror.l	#8,d1
		moveq.l	#1,d2
PrintHexLoop:
		rol.l	#4,d1
		move.b	d1,d0
		andi.b	#0xF,d0
		addq.b	#1,d0
		move.w	d0,(a6)
		dbf		d2,PrintHexLoop
		rts

ErrFont:
		dc.b	0,0,0								/* ' ' : 255 */
		dc.b	0b01110101, 0b01010101, 0b01010111	/* 0 : 00 */
		dc.b	0b00100010, 0b00100010, 0b00100010	/* 1 : 01 */
		dc.b	0b01110001, 0b01110100, 0b01000111	/* 2 : 02 */
		dc.b	0b01110001, 0b00110001, 0b00010111	/* 3 : 03 */
		dc.b	0b01010101, 0b01010111, 0b00010001	/* 4 : 04 */
		dc.b	0b01110100, 0b01110001, 0b00010111	/* 5 : 05 */
		dc.b	0b01110100, 0b01110101, 0b01010111	/* 6 : 06 */
		dc.b	0b01110001, 0b10010010, 0b00100010	/* 7 : 07 */
		dc.b	0b01110101, 0b01110101, 0b01010111	/* 8 : 08 */
		dc.b	0b01110101, 0b01010111, 0b00010111	/* 9 : 09 */
		dc.b	0b01110101, 0b01010111, 0b01010101	/* A : 10 */
		dc.b	0b11110101, 0b11110101, 0b01011111	/* B : 11 */
		dc.b	0b01110100, 0b01000100, 0b01000111	/* C : 12 */
		dc.b	0b11110101, 0b01010101, 0b01011111	/* D : 13 */
		dc.b	0b01110100, 0b01100100, 0b01000111	/* E : 14 */
		dc.b	0b01110100, 0b01100100, 0b01000100	/* F : 15 */
		dc.b	0b01110100, 0b01000101, 0b01010111	/* G : 16 */
		dc.b	0b10110010, 0b00100010, 0b00101011	/* I : 17 */
		dc.b	0b01000100, 0b01000100, 0b01000111	/* L : 18 */
		dc.b	0b11110101, 0b01010101, 0b01010101	/* N : 19 */
		dc.b	0b10110101, 0b01010101, 0b01011011	/* O : 20 */
		dc.b	0b11110101, 0b01011111, 0b01000100	/* P : 21 */
		dc.b	0b11110101, 0b01011111, 0b01010101	/* R : 22 */
		dc.b	0b10110101, 0b10101001, 0b01011011	/* S : 23 */
		dc.b	0b01110010, 0b00100010, 0b00100010	/* T : 24 */
		dc.b	0b01010101, 0b01010101, 0b01011011	/* U : 25 */
		dc.b	0b01010101, 0b01010101, 0b10110010	/* V : 26 */
		dc.b	0b01110001, 0b10010010, 0b10100111	/* Z : 27 */
		dc.b	0b00000111, 0b00000000, 0b01110000	/* = : 28 */
		dc.b	0b00100010, 0b01110111, 0b00100010	/* + : 29 */
		.even
