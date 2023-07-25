// SFX for IKD

const char fire[] PROGMEM = {
  0, PC_ENV_VOL, 255,
  0, PC_NOISE_PARAMS, 1,
  0, PC_ENV_SPEED, -9,
  0, PATCH_END,
};

const char explode[] PROGMEM = {
	0,PC_ENV_VOL,0xff,
	0,PC_ENV_SPEED,-2,
	0,PC_NOISE_PARAMS,0x1,
	10,PC_ENV_VOL,0xa0,
	10,PC_ENV_VOL,0x80,
	10,PC_ENV_VOL,0x60,
	0,PATCH_END
};


const char engine[] PROGMEM ={
0,PC_WAVE,6,
3,PC_ENV_SPEED,-10,
0, PC_NOTE_CUT, 0,
0,PATCH_END
};

const char boing[] PROGMEM ={
0,PC_WAVE,8,
0,PC_ENV_VOL,0xE0,
0,PC_ENV_SPEED,-20,
2,PC_NOTE_DOWN,6,
2,PC_NOTE_DOWN,6,
2,PC_NOTE_CUT,0,
0,PATCH_END
};

const struct PatchStruct patches[] PROGMEM = {
  {1, NULL, fire, 0, 0},
  {1, NULL, explode, 0, 0},
  {0, NULL, engine, 0, 0},
  {0, NULL, boing, 0, 0},
};
