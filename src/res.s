#include "md.h"

.macro BIN _name, _file, _align=2
	.globl \_name
	.globl \_name\()_end
	.align \_align
\_name:
	.incbin "\_file"
\_name\()_end:
.endm

.section .text

BIN spr_arrow,	"res/spr/arrow.spr"
BIN spr_arrow2,	"res/spr/arrow2.spr"
BIN spr_oldguy,	"res/spr/oldguy.spr"
BIN spr_what,	"res/spr/what.spr"
BIN spr_what2,	"res/spr/what2.spr"

BIN pat_green,	"res/bg/green.pat"
BIN map_green,	"res/bg/green.map"
BIN pat_pink,	"res/bg/pink.pat"
BIN map_pink,	"res/bg/pink.map"
BIN pat_sky,	"res/bg/sky.pat"
BIN map_sky,	"res/bg/sky.map"
BIN pat_flash,	"res/bg/flash.pat"
BIN map_flash,	"res/bg/flash.map"

BIN pcm_data,	"res/monsquaz_22050_s8.pcm", 256

BIN pat_bigguy,	"res/bg2/bigguy.pat"
BIN map_bigguy,	"res/bg2/bigguy.map"
