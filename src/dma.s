#include "md.h"

.section .bss

dmabuf:		ds.w	7*48
dmabuf_end:	ds.l	1

.section .text

	.global dma_queue
dma_queue:
		move.l	(dmabuf_end),a0

		move.l	8(sp), d0 /* from */
		move.w	14(sp),d1 /* len */

		lsr.l	#1,d0
		move.l	#0x94009300, (a0)+
		move.l	#0x97009600, (a0)+
		move.w	#0x9500, (a0)+
		movep.l	d0, -7(a0)
		movep.w	d1, -9(a0)

		move.l	4(sp), d0 /* cmd */
		move.w	12(sp),d1 /* to */

		and.l	#0xFFFF,d1
		lsl.l	#2,d1
		lsr.w	#2,d1
		swap	d1
		or.l	d0,d1
		move.l	d1,(a0)+

		move.l	a0,(dmabuf_end)
		rts

	.global dma_flush
dma_flush:
		lea		(dmabuf),a0
		lea		(VDP_CTRL_PORT),a1

		move.l	(dmabuf_end),d0
		sub.l	#dmabuf,d0
		beq.s	dma_clear
		lsr.w	#1,d0
		subq.w	#1,d0

		FAST_PAUSE_Z80

	dma_flush_loop:
		move.w	(a0)+,(a1)
		dbf		d0,dma_flush_loop

		RESUME_Z80

	.global dma_clear
dma_clear:
		move.l	#dmabuf,(dmabuf_end)
		rts
