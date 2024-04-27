#include "md.h"
#include "res.h"

#define DmaVRAM		0x40000080LU
#define DmaCRAM		0xC0000080LU
#define DmaVSRAM	0x40000090LU

extern void pcm_start(void);

extern void dma_queue(uint32_t cmd, uint32_t from, uint16_t to, uint16_t len);
extern void dma_flush();
extern void dma_clear();

static uint16_t sprite_count;
static Sprite sprite_table[80];

void vdp_tiles(const uint32_t *data, uint16_t index, uint16_t num) {
	dma_queue(DmaVRAM, (uint32_t) data, index << 5, num << 4);
}

void vdp_map_hline(uint16_t plan, const uint16_t *tiles, uint16_t x, uint16_t y, uint16_t len) {
	dma_queue(DmaVRAM, (uint32_t) tiles, plan + ((x + (y << PLAN_WIDTH_SFT)) << 1), len);
}

void vdp_colors(uint16_t index, const uint16_t *values, uint16_t count) {
	dma_queue(DmaCRAM, (uint32_t) values, index << 1, count);
}

void vdp_vscroll(uint16_t plan, int16_t vscroll) {
	uint32_t addr = (plan == VDP_PLANE_A) ? 0 : 2;
	*vdp_ctrl_wide = ((0x4000 + ((addr) & 0x3FFF)) << 16) + (((addr) >> 14) | 0x10);
	*vdp_data_port = vscroll;
}

void vdp_sprite_add(const Sprite *spr) {
	sprite_table[sprite_count] = *spr;
	sprite_table[sprite_count].link = sprite_count + 1;
	sprite_count++;
}

void vdp_sprites_add(const Sprite *spr, uint16_t num) {
	for(uint16_t i = num; i--;) vdp_sprite_add(&spr[i]);
}

void vdp_sprites_clear() {
	static const Sprite NULL_SPRITE = {.x = 0x80, .y = 0x80};
	sprite_count = 0;
	vdp_sprites_add(&NULL_SPRITE, 1);
}

void vdp_sprites_update() {
	if(!sprite_count) return;
	sprite_table[sprite_count - 1].link = 0; // Mark end of sprite list
	dma_queue(DmaVRAM, (uint32_t) sprite_table, VDP_SPRITE_TABLE, sprite_count << 2);
	sprite_count = 0;
}

#define vdp_vsync()						\
	__asm__("clr.b	(v_vbl_flag)\n"		\
			"11:\n\t"					\
			"tst.b	(v_vbl_flag)\n\t"	\
			"beq.s	11b")

#define TI_BACK		1
#define TI_OLDGUY	(TI_BACK+750)
#define TI_ZOOMIN	(TI_BACK+234)

#define TI_ARROW	((VDP_PLANE_A>>5)-40)
#define TI_FARROW	((VDP_PLANE_A>>5)-80)
#define TI_WHAT		((VDP_PLANE_B>>5)-115)

// including pat_sky_size here pulled in __udivsi3
#define SKY_SIZE	(/*pat_sky_size / 32*/ 750 / 3)

typedef struct {
	int16_t x, y;		// Position of object (upper left of meta sprite)
	const Sprite *sdef;	// Pointer to one of the sprite definitions above
	uint16_t width;		// Total meta sprite width in pixels
	uint16_t snum;		// Total number of VDP sprites
	uint16_t attr;		// Tile attributes and VRAM tile index of first tile
} Object;

void obj_draw(Object *obj) {
	for(uint16_t i = 0; i < obj->snum; i++) {
		Sprite spr;
		if(obj->attr & 0x800) { // HFLIP
			spr.x = obj->x + obj->width + 128;
			spr.x -= obj->sdef[i].x + ((obj->sdef[i].size & 0xC) << 1) + 8;
		} else {
			spr.x = obj->x + obj->sdef[i].x + 128;
		}
		spr.y = obj->y + obj->sdef[i].y + 128;
		spr.size = obj->sdef[i].size;
		spr.attr = obj->attr + obj->sdef[i].attr;
		vdp_sprite_add(&spr);
	}
}

Object oldguy, guyarrow, larrow, rarrow, lwhat, rwhat;

void green_arrows_up(uint16_t timer) {
	oldguy.y -= 4;
	larrow.y -= 2;
	rarrow.y -= 2;
	guyarrow.x = oldguy.x + 33;
	guyarrow.y = oldguy.y + 122;
	obj_draw(&guyarrow);
	obj_draw(&oldguy);
	obj_draw(&larrow);
	obj_draw(&rarrow);
	vdp_sprites_update();
}

void back_and_forth(uint16_t timer) {
	switch(timer & 0x60) {
	case 0x00:
		oldguy.x -= 2; oldguy.y += 1; 
		lwhat.y += 2; rwhat.y -= 2;
		break;
	case 0x20:
		oldguy.x += 2; oldguy.y -= 1; 
		lwhat.y -= 2; rwhat.y += 2;
		break;
	case 0x40:
		oldguy.x += 2; oldguy.y += 1;
		lwhat.y += 2; rwhat.y -= 2;
		break;
	case 0x60:
		oldguy.x -= 2; oldguy.y -= 1;
		lwhat.y -= 2; rwhat.y += 2;
		break;
	}
	guyarrow.x = oldguy.x + 33;
	guyarrow.y = oldguy.y + 122;
	obj_draw(&guyarrow);
	obj_draw(&oldguy);
	obj_draw(&lwhat);
	obj_draw(&rwhat);
	vdp_sprites_update();
}

void pink_arrows_down(uint16_t timer) {
	oldguy.y += 4;
	guyarrow.x = oldguy.x + 33;
	guyarrow.y = oldguy.y + 122;
	larrow.y += 2;
	rarrow.y += 2;
	obj_draw(&guyarrow);
	obj_draw(&oldguy);
	obj_draw(&larrow);
	obj_draw(&rarrow);
	vdp_sprites_update();
}

void zooming_in(uint16_t timer) {
	uint16_t frame = timer >> 2;
	if(!(timer & 3)) {
		vdp_colors(0, pal_flash[frame & 15], 16);
		if(frame < 27) vdp_map_hline(VDP_PLANE_A, map_bigguy+(frame*25), 6, frame, 25);
		if(frame < 25) vdp_tiles(pat_bigguy+frame*8*16, TI_ZOOMIN+frame*16, 16);
	}
	int16_t scroll = -224+timer*2;
	vdp_vscroll(VDP_PLANE_A, scroll - frame);
}

#define STATE1_TIME	(62*2)
#define STATE2_TIME	(119*2)
#define STATE3_TIME	(180*2)
#define END_TIME	(242*2)

void objects_init(void) {
	oldguy.sdef = sdef_oldguy;
	oldguy.width = 96;
	oldguy.snum = 14;
	oldguy.attr = SA(3,0,0,TI_OLDGUY);

	vdp_tiles(spr_arrow, TI_ARROW, 40);
	larrow.width = 56;
	larrow.snum = 3;
	larrow.attr = SA(2,0,0,TI_ARROW);
	rarrow.width = 56;
	rarrow.snum = 3;
	rarrow.attr = SA(2,0,0,TI_ARROW);

	vdp_tiles(spr_arrow2, TI_FARROW, 40);
	guyarrow.sdef = sdef_guy_arrow;
	guyarrow.width = 64;
	guyarrow.snum = 4;
	guyarrow.attr = SA(2,0,0,TI_FARROW);

	vdp_tiles(spr_what, TI_WHAT, 115);
	lwhat.sdef = sdef_what;
	lwhat.width = 64;
	lwhat.snum = 11;
	lwhat.attr = SA(2,0,0,TI_WHAT);
	rwhat.sdef = sdef_what;
	rwhat.width = 64;
	rwhat.snum = 11;
	rwhat.attr = SA(2,0,0,TI_WHAT);
}

void main(void) {
	uint16_t pal_system = *vdp_ctrl_port & 1;
	dma_clear();
	objects_init();
	vdp_colors(28, &pal_what[16], 4);
	vdp_colors(32, pal_what, 16);
	vdp_colors(48, pal_oldguy, 16);
	uint16_t timer = 0;
	uint16_t state = 0;
	ENABLE_VINT;
	for(;;) {
		if(!pal_system || (timer % 6 != 5)) {
			vdp_vsync();
		}
		dma_flush();
		switch(timer) {
		default: break;
		case 0: // Green arrows up
			*vdp_ctrl_port = 0x8200 | (VDP_PLANE_B >> 10); // Hide Plane A by pointing it to Plane B
			vdp_vscroll(VDP_PLANE_A, 0);
			pcm_start();
			vdp_tiles(pat_green, TI_BACK, pat_green_size / 8);
			for(uint16_t i = 0; i < 28; i++) {
				vdp_map_hline(VDP_PLANE_B, &map_green[i*40], 0, i, 40);
			}
			vdp_tiles(spr_oldguy, TI_OLDGUY, 176);
			vdp_colors(0, pal_green, 16);
			DISPLAY_ON;
			larrow.sdef = sdef_green_arrow;
			rarrow.sdef = sdef_green_arrow;
			oldguy.x = 160 - 48;
			oldguy.y = 224;
			larrow.x = 32;
			larrow.y = 192;
			rarrow.x = 240;
			rarrow.y = 160;
			larrow.attr &= ~0x6000; // Change arrow pals to PAL2
			larrow.attr |= 0x4000;
			rarrow.attr &= ~0x6000;
			rarrow.attr |= 0x4000;
			state = 1;
			break;
		case 1: // Clear plane A in the background
			for(uint16_t i = 0; i < 15; i++) {
				vdp_map_hline(VDP_PLANE_A, (const uint16_t*) blank_data, 6, i, 25);
			}
			break;
		case 2:
			for(uint16_t i = 15; i < 27; i++) {
				vdp_map_hline(VDP_PLANE_A, (const uint16_t*) blank_data, 6, i, 25);
			}
			
			break;
		case STATE1_TIME-1: // Sky background tile loading split to 3 frames
			DISPLAY_OFF;
			vdp_colors(0, (const uint16_t*) blank_data, 16);
			vdp_tiles(pat_sky, TI_BACK, SKY_SIZE);
			for(uint16_t i = 0; i < 10; i++) {
				vdp_map_hline(VDP_PLANE_B, &map_sky[i*40], 0, i, 40);
			}
			break;
		case STATE1_TIME: // Back and forth
			state = 2;
			vdp_tiles(&pat_sky[SKY_SIZE*8], TI_BACK + SKY_SIZE, SKY_SIZE);
			for(uint16_t i = 10; i < 20; i++) {
				vdp_map_hline(VDP_PLANE_B, &map_sky[i*40], 0, i, 40);
			}
			oldguy.x = 160 - 48;
			oldguy.y = 48;
			lwhat.x = 8;
			lwhat.y = 96-32;
			rwhat.x = 320-64-8;
			rwhat.y = 96+32;
			break;
		case STATE1_TIME+1:
			vdp_tiles(&pat_sky[SKY_SIZE*2*8], TI_BACK + SKY_SIZE*2, SKY_SIZE);
			for(uint16_t i = 20; i < 28; i++) {
				vdp_map_hline(VDP_PLANE_B, &map_sky[i*40], 0, i, 40);
			}
			vdp_colors(0, pal_sky, 16);
			DISPLAY_ON;
			break;
		case STATE2_TIME: // Pink ones are coming down
			vdp_tiles(pat_pink, TI_BACK, pat_pink_size / 8);
			for(uint16_t i = 0; i < 28; i++) {
				vdp_map_hline(VDP_PLANE_B, &map_pink[i*40], 0, i, 40);
			}
			vdp_colors(0, pal_pink, 16);
			larrow.sdef = sdef_red_arrow;
			rarrow.sdef = sdef_red_arrow;
			oldguy.x = 160 - 48;
			oldguy.y = -192;
			larrow.x = 32;
			larrow.y = -16;
			rarrow.x = 240;
			rarrow.y = 8;
			larrow.attr &= ~0x6000; // Change arrow pals to PAL1
			larrow.attr |= 0x2000;
			rarrow.attr &= ~0x6000;
			rarrow.attr |= 0x2000;
			state = 3;
			break;
		case STATE3_TIME: // Zooming in
			state = 4;
			vdp_vscroll(VDP_PLANE_A, -224);
			*vdp_ctrl_port = 0x8200 | (VDP_PLANE_A >> 10); // Show plane A
			vdp_tiles(pat_flash, TI_BACK, pat_flash_size / 8);
			for(uint16_t i = 0; i < 28; i++) {
				vdp_map_hline(VDP_PLANE_B, &map_flash[i*40], 0, i, 40);
			}
			vdp_sprites_clear();
			vdp_sprites_update();
			break;
		}
		switch(state) {
		default: break;
		case 1: green_arrows_up(timer);					break;
		case 2: back_and_forth(timer - STATE1_TIME);	break;
		case 3: pink_arrows_down(timer - STATE2_TIME);	break;
		case 4: zooming_in(timer - STATE3_TIME);		break;
		}
		if(++timer > END_TIME) timer = 0;
	}
}
