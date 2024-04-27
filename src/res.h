extern const uint32_t blank_data[];

extern const uint32_t spr_arrow[];
extern const uint32_t spr_arrow2[];
extern const uint32_t spr_oldguy[];
extern const uint32_t spr_what[];
extern const uint32_t spr_what2[];

extern const uint32_t spr_arrow_end[];
extern const uint32_t spr_arrow2_end[];
extern const uint32_t spr_oldguy_end[];
extern const uint32_t spr_what_end[];
extern const uint32_t spr_what2_end[];

#define spr_arrow_size	(spr_arrow_end-spr_arrow)
#define spr_arrow2_size	(spr_arrow2_end-spr_arrow2)
#define spr_oldguy_size	(spr_oldguy_end-spr_oldguy)
#define spr_what_size	(spr_what_end-spr_what)
#define spr_what2_size	(spr_what2_end-spr_what2)

extern const uint32_t pat_green[];
extern const uint16_t map_green[];
extern const uint32_t pat_pink[];
extern const uint16_t map_pink[];
extern const uint32_t pat_sky[];
extern const uint16_t map_sky[];
extern const uint32_t pat_flash[];
extern const uint16_t map_flash[];

extern const uint32_t pat_green_end[];
extern const uint16_t map_green_end[];
extern const uint32_t pat_pink_end[];
extern const uint16_t map_pink_end[];
extern const uint32_t pat_sky_end[];
extern const uint16_t map_sky_end[];
extern const uint32_t pat_flash_end[];
extern const uint16_t map_flash_end[];

#define pat_green_size	(pat_green_end-pat_green)
#define map_green_size	(map_green_end-map_green)
#define pat_pink_size	(pat_pink_end-pat_pink)
#define map_pink_size	(map_pink_end-map_pink)
#define pat_sky_size	(pat_sky_end-pat_sky)
#define map_sky_size	(map_sky_end-map_sky)
#define pat_flash_size	(pat_flash_end-pat_flash)
#define map_flash_size	(map_flash_end-map_flash)

extern const uint32_t pat_bigguy[];
extern const uint16_t map_bigguy[];

extern const uint32_t pat_bigguy_end[];
extern const uint16_t map_bigguy_end[];

#define pat_bigguy_size	(pat_bigguy_end-pat_bigguy)
#define map_bigguy_size	(map_bigguy_end-map_bigguy)

// Palettes

static const uint16_t pal_oldguy[16] = {
	0x000, 0x000, 0x022, 0x224, 0x244, 0x246, 0x246, 0x468,
	0x48A, 0x468, 0x468, 0x68A, 0x68A, 0x46A, 0x46A, 0x68A,
};
static const uint16_t pal_green[16] = {
	0x000, 0x000, 0x020, 0x040, 0x260, 0x460, 0x480, 0x4A0,
	0x6A2, 0x6C2, 0x8C4, 0x8E4, 0xAE6, 0xAE8, 0xAEC, 0xEEE,
};
static const uint16_t pal_pink[16] = {
	0x000, 0x000, 0x002, 0x004, 0x206, 0x406, 0x408, 0x40A,
	0x62A, 0x62C, 0x84C, 0x84E, 0xA6E, 0xA8E, 0xACE, 0xEEE,
};
static const uint16_t pal_sky[16] = {
	0x000, 0x022, 0x422, 0x642, 0x644, 0x444, 0x466, 0x666,
	0x864, 0x666, 0x886, 0x688, 0x48A, 0x8AA, 0xACC, 0x8CE,
};
static const uint16_t pal_what[20] = {
	0x000, 0x000, 0x200, 0x002, 0x004, 0x222, 0x444, 0x246,
	0x668, 0x68A, 0x68C, 0x8AC, 0x280, 0x4A0, 0x6C0, 0x8E0,
	0x208, 0x40A, 0x62C, 0x82E, /* pink version of last 4 colors */
};

// Palette cycle for last part

#define _BK0_ 0xA00
#define _BK1_ 0xA02
#define _BK2_ 0x804
#define _BK3_ 0x806
#define _BK4_ 0x608
#define _BK5_ 0x40A
#define _BK6_ 0x40C
#define _BK7_ 0x20E
#define _BK8_ 0x00E
#define _BK9_ 0x20E
#define _BKA_ 0x40C
#define _BKB_ 0x40A
#define _BKC_ 0x608
#define _BKD_ 0x806
#define _BKE_ 0x804
#define _BKF_ 0xA02

#define GREEN 0x0E0
#define YELLO 0x0EE
#define WHITE 0xEEE

static const uint16_t pal_flash[16][16] = {
	{
		_BK0_, GREEN, _BK0_, _BK0_, _BK0_, _BK0_, _BK0_, _BK0_,
		_BK0_, _BK0_, _BK0_, _BK0_, _BK0_, _BK0_, _BK0_, WHITE,
	},{
		_BK1_, _BK1_, GREEN, YELLO, _BK1_, _BK1_, _BK1_, _BK1_,
		_BK1_, _BK1_, _BK1_, _BK1_, _BK1_, _BK1_, _BK1_, WHITE,
	},{
		_BK2_, _BK2_, _BK2_, _BK2_, GREEN, YELLO, _BK2_, _BK2_,
		_BK2_, _BK2_, _BK2_, _BK2_, _BK2_, _BK2_, _BK2_, WHITE,
	},{
		_BK3_, _BK3_, _BK3_, _BK3_, _BK3_, _BK3_, GREEN, YELLO,
		_BK3_, _BK3_, _BK3_, _BK3_, _BK3_, _BK3_, _BK3_, WHITE,
	},{
		_BK4_, _BK4_, _BK4_, _BK4_, _BK4_, _BK4_, _BK4_, _BK4_,
		GREEN, YELLO, _BK4_, _BK4_, _BK4_, _BK4_, _BK4_, WHITE,
	},{
		_BK5_, _BK5_, _BK5_, _BK5_, _BK5_, _BK5_, _BK5_, _BK5_,
		_BK5_, _BK5_, GREEN, YELLO, _BK5_, _BK5_, _BK5_, WHITE,
	},{
		_BK6_, _BK6_, _BK6_, _BK6_, _BK6_, _BK6_, _BK6_, _BK6_,
		_BK6_, _BK6_, _BK6_, _BK6_, GREEN, YELLO, _BK6_, WHITE,
	},{
		_BK7_, _BK7_, _BK7_, _BK7_, _BK7_, _BK7_, _BK7_, _BK7_,
		_BK7_, _BK7_, _BK7_, _BK7_, _BK7_, _BK7_, GREEN, WHITE,
	},{
		_BK8_, _BK8_, _BK8_, _BK8_, _BK8_, _BK8_, _BK8_, _BK8_,
		_BK8_, _BK8_, _BK8_, _BK8_, _BK8_, _BK8_, YELLO, WHITE,
	},{
		_BK9_, _BK9_, _BK9_, _BK9_, _BK9_, _BK9_, _BK9_, _BK9_,
		_BK9_, _BK9_, _BK9_, _BK9_, YELLO, GREEN, _BK9_, WHITE,
	},{
		_BKA_, _BKA_, _BKA_, _BKA_, _BKA_, _BKA_, _BKA_, _BKA_,
		_BKA_, _BKA_, YELLO, GREEN, _BKA_, _BKA_, _BKA_, WHITE,
	},{
		_BKB_, _BKB_, _BKB_, _BKB_, _BKB_, _BKB_, _BKB_, _BKB_,
		YELLO, GREEN, _BKB_, _BKB_, _BKB_, _BKB_, _BKB_, WHITE,
	},{
		_BKC_, _BKC_, _BKC_, _BKC_, _BKC_, _BKC_, YELLO, GREEN,
		_BKC_, _BKC_, _BKC_, _BKC_, _BKC_, _BKC_, _BKC_, WHITE,
	},{
		_BKD_, _BKD_, _BKD_, _BKD_, YELLO, GREEN, _BKD_, _BKD_,
		_BKD_, _BKD_, _BKD_, _BKD_, _BKD_, _BKD_, _BKD_, WHITE,
	},{
		_BKE_, _BKE_, YELLO, GREEN, _BKE_, _BKE_, _BKE_, _BKE_,
		_BKE_, _BKE_, _BKE_, _BKE_, _BKE_, _BKE_, _BKE_, WHITE,
	},{
		_BKF_, YELLO, _BKF_, _BKF_, _BKF_, _BKF_, _BKF_, _BKF_,
		_BKF_, _BKF_, _BKF_, _BKF_, _BKF_, _BKF_, _BKF_, WHITE,
	},
};

// Sprite Lists

#define SS(w,h)     (((w-1)<<2)|(h-1))
#define SA(p,v,h,i) (((p)<<13)|((v)<<12)|((h)<<11)|(i))

static const Sprite sdef_oldguy[14] = {
	{ .x=47, .y=  0, .size=SS(4,4), .attr=SA(0,0,0,  0) },
	{ .x=17, .y= 32, .size=SS(4,4), .attr=SA(0,0,0, 16) },
	{ .x=49, .y= 32, .size=SS(4,4), .attr=SA(0,0,0, 32) },
	{ .x= 0, .y= 64, .size=SS(4,4), .attr=SA(0,0,0, 48) },
	{ .x=32, .y= 64, .size=SS(4,4), .attr=SA(0,0,0, 64) },
	{ .x=64, .y= 64, .size=SS(4,4), .attr=SA(0,0,0, 80) },
	{ .x=32, .y= 96, .size=SS(4,4), .attr=SA(0,0,0, 96) },
	{ .x=24, .y=160, .size=SS(4,4), .attr=SA(0,0,0,112) },
	{ .x=56, .y=160, .size=SS(4,4), .attr=SA(0,0,0,128) },
	{ .x=64, .y= 96, .size=SS(3,4), .attr=SA(0,0,0,144) },
	{ .x=24, .y=128, .size=SS(2,4), .attr=SA(0,0,0,156) },
	{ .x=40, .y=128, .size=SS(4,2), .attr=SA(0,0,0,164) },
	{ .x= 0, .y= 96, .size=SS(3,1), .attr=SA(0,0,0,172) },
	{ .x=75, .y=128, .size=SS(1,1), .attr=SA(0,0,0,175) },
};

static const Sprite sdef_what[11] = {
	{ .x= 0, .y= 48, .size=SS(4,4), .attr=SA(0,0,0,  0) },
	{ .x=32, .y= 48, .size=SS(4,4), .attr=SA(0,0,0, 16) },
	{ .x= 0, .y= 80, .size=SS(4,4), .attr=SA(0,0,0, 32) },
	{ .x=32, .y= 80, .size=SS(3,4), .attr=SA(0,0,0, 48) },
	{ .x= 8, .y=112, .size=SS(3,4), .attr=SA(0,0,0, 60) },
	{ .x=32, .y=112, .size=SS(2,4), .attr=SA(0,0,0, 72) },
	{ .x= 8, .y=144, .size=SS(2,2), .attr=SA(0,0,0, 80) },
	{ .x=32, .y=144, .size=SS(2,2), .attr=SA(0,0,0, 84) },
	{ .x=16, .y=  0, .size=SS(3,3), .attr=SA(0,0,0, 88) },
	{ .x= 8, .y= 24, .size=SS(3,3), .attr=SA(0,0,0, 97) },
	{ .x=32, .y= 24, .size=SS(3,3), .attr=SA(0,0,0,106) },
};

static const Sprite sdef_green_arrow[3] = {
	{ .x= 0, .y=  0, .size=SS(4,4), .attr=SA(0,0,0,  0) },
	{ .x=32, .y=  0, .size=SS(3,4), .attr=SA(0,0,0, 16) },
	{ .x=16, .y= 32, .size=SS(3,4), .attr=SA(0,0,0, 28) },
};
static const Sprite sdef_red_arrow[3] = {
	{ .x= 0, .y= 32, .size=SS(4,4), .attr=SA(0,1,0,  0) },
	{ .x=32, .y= 32, .size=SS(3,4), .attr=SA(0,1,0, 16) },
	{ .x=16, .y=  0, .size=SS(3,4), .attr=SA(0,1,0, 28) },
};

static const Sprite sdef_guy_arrow[4] = {
	{ .x= 0, .y= 16, .size=SS(4,3), .attr=SA(0,0,0,  0) },
	{ .x=32, .y=  0, .size=SS(4,3), .attr=SA(0,0,0, 12) },
	{ .x=32, .y= 24, .size=SS(4,3), .attr=SA(0,0,0, 24) },
	{ .x=32, .y= 48, .size=SS(2,1), .attr=SA(0,0,0, 36) },
};
