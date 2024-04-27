#define Z80_RAM				0xA00000		/* Where Z80 RAM starts */
#define Z80_DRV_COMMAND		0xA00100
#define Z80_DRV_STATUS		0xA00102
#define Z80_DRV_PARAMS		0xA00104

#define Z80_BUS_REQ			0xA11100		/* Z80 bus request line */
#define Z80_RESET			0xA11200		/* Z80 reset line */

#define VDP_DATA_PORT		0xC00000
#define VDP_CTRL_PORT		0xC00004
#define VDP_COUNTER			0xC00008

#define VDP_PLANE_W			0xB000U
#define VDP_PLANE_A			0xC000U
#define VDP_PLANE_B			0xE000U
#define VDP_SPRITE_TABLE	0xF800U
#define VDP_HSCROLL_TABLE	0xFC00U

#define PLAN_WIDTH			64
#define PLAN_HEIGHT			32
#define PLAN_WIDTH_SFT		6
#define PLAN_HEIGHT_SFT		5


#ifdef __ASSEMBLER__

#define PAUSE_Z80						;\
		move.w	#0x100,(Z80_BUS_REQ)	;\
	102:								;\
		btst	#0,(Z80_BUS_REQ)		;\
		bne.s	102b
#define FAST_PAUSE_Z80	move.w #0x100,(Z80_BUS_REQ)
#define RESUME_Z80		move.w #0x000,(Z80_BUS_REQ)

#else

#include <stddef.h>
#include <stdint.h>

typedef struct {
	int16_t y;
	uint8_t size;
	uint8_t link;
	uint16_t attr;
	int16_t x;
} Sprite;

#define DISABLE_INTS	__asm__("move #0x2700,%sr")
#define ENABLE_VINT		__asm__("move #0x2500,%sr")

#define DISPLAY_ON		__asm__("move.w #0x8174,0xC00004");
#define DISPLAY_OFF		__asm__("move.w #0x8134,0xC00004");

static volatile uint16_t *const vdp_data_port = (uint16_t *) 0xC00000;
static volatile uint16_t *const vdp_ctrl_port = (uint16_t *) 0xC00004;
static volatile uint32_t *const vdp_ctrl_wide = (uint32_t *) 0xC00004;

#endif
