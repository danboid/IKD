const char patch00[] PROGMEM ={//bass drum?(another hat?)
0,PC_ENV_VOL,34+20,
0,PC_ENV_SPEED,-4-3,
0,PC_NOISE_PARAMS,5,
1,PC_NOISE_PARAMS,2,
1,PC_NOISE_PARAMS,1,
1,PC_NOISE_PARAMS,2,
0,PC_ENV_SPEED,-6-4,
1,PC_NOISE_PARAMS,5,
1,PC_NOISE_PARAMS,1,
1,PC_NOISE_PARAMS,2,
2,PC_NOTE_CUT,0,
0,PATCH_END
  };

const char patch01[] PROGMEM ={//snare
0,PC_ENV_VOL,78+60,
0,PC_NOISE_PARAMS,10,
1,PC_NOISE_PARAMS,5,
1,PC_NOISE_PARAMS,3,
1,PC_NOISE_PARAMS,1,
0,PC_ENV_SPEED,-12,
1,PC_NOISE_PARAMS,5,
1,PC_NOISE_PARAMS,3,
1,PC_NOISE_PARAMS,1,
2,PC_NOTE_CUT,0,
0,PATCH_END
  };


const char patch02[] PROGMEM ={//hats
0,PC_ENV_VOL,46+20,
0,PC_ENV_SPEED,-4-2,
0,PC_NOISE_PARAMS,5,
1,PC_NOISE_PARAMS,2,
1,PC_NOISE_PARAMS,1,
1,PC_NOISE_PARAMS,2,
1,PC_NOISE_PARAMS,1,
1,PC_NOISE_PARAMS,2,
0,PC_ENV_SPEED,-6-3,
1,PC_NOISE_PARAMS,5,
1,PC_NOISE_PARAMS,1,
1,PC_NOISE_PARAMS,2,
1,PC_NOISE_PARAMS,5,
2,PC_NOTE_CUT,0,
0,PATCH_END
};

const char patch03[] PROGMEM ={//tom tom low(channel 5)
//0,PC_WAVE,4,
0,PC_ENV_VOL,240,
0,PC_PITCH,59,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_CUT,0,
0,PATCH_END
  };

const char patch04[] PROGMEM ={//tom tom high(channel 5)
//0,PC_WAVE,4,
0,PC_ENV_VOL,240,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_CUT,0,
0,PATCH_END
  };

const char patch05[] PROGMEM ={//crash 1
0,PC_NOISE_PARAMS,2,
0,PC_ENV_VOL,255,
0,PC_ENV_SPEED,-12,
1,PC_NOISE_PARAMS,1,
1,PC_NOISE_PARAMS,2,
2,PC_NOISE_PARAMS,1,
5,PC_ENV_SPEED,-24,
64,PC_NOTE_CUT,0,
0,PATCH_END
  };

const char patch06[] PROGMEM ={//crash 2
0,PC_NOISE_PARAMS,9,
0,PC_ENV_VOL,255,
1,PC_NOISE_PARAMS,12,
1,PC_ENV_SPEED,-12,
1,PC_ENV_SPEED,-24,
1,PC_ENV_SPEED,-30,
64,PC_NOTE_CUT,0,
0,PATCH_END
  };

const char patch07[] PROGMEM ={//bass guitar normal(bass-envelope.raw 8 frames from attack to sustain, 1 frame release to silence)
0,PC_WAVE,4,
0,PC_ENV_VOL,90,
0,PC_ENV_SPEED,46,
1,PC_ENV_SPEED,0,//8

//0,PC_NOTE_HOLD,0,

9,PC_ENV_SPEED,-24,
2,PC_NOTE_CUT,0,
0,PATCH_END

  };

const char patch08[] PROGMEM ={//bass guitar fast bridge section
0,PC_WAVE,4,
0,PC_ENV_VOL,120,
//0,PC_TREMOLO_RATE,24,
//0,PC_TREMOLO_LEVEL,151,
2,PC_ENV_SPEED,-6,
4,PC_ENV_SPEED,0,
//0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-6,
0,PATCH_END
  };

const char patch09[] PROGMEM ={//rythm modulated accent
0,PC_WAVE,8,
0,PC_ENV_VOL,100-20,
0,PC_TREMOLO_RATE,120,
0,PC_TREMOLO_LEVEL,128,
0,PC_ENV_SPEED,42-10,
2,PC_ENV_SPEED,-2,
0,PC_LOOP_START,23,
1,PC_WAVE,9,
0,PC_NOTE_DOWN,12,
1,PC_WAVE,8,
0,PC_NOTE_UP,12,
0,PC_LOOP_END,4,
1,PC_ENV_SPEED,-4,//19
3,PC_ENV_SPEED,-10,
0,PC_TREMOLO_RATE,5,//added
0,PC_TREMOLO_LEVEL,255,//added
0,PC_WAVE,7,//added
7,PC_ENV_SPEED,-10,//4,-12
15,PC_NOTE_CUT,0,//5
0,PATCH_END

  };



const char patch10[] PROGMEM ={//rythm main(original pattern is roughly 9-10 double length(lower note), for 17-19 shorter(half length, 1octave up)
0,PC_WAVE,3,//4
0,PC_ENV_VOL,140,//216,//80

2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
0,PC_ENV_SPEED,-3,//2,-6
0,PC_ENV_SPEED,0,
//0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-6,

0,PC_LOOP_START,8+1,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
0,PC_LOOP_END,2,
0,PC_ENV_SPEED,-12,//3,-20

0,PC_LOOP_START,17+1,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
0,PC_LOOP_END,2,
255,PC_NOTE_CUT,0,
0,PATCH_END

  };


const char patch11[] PROGMEM ={//lead(was rythm) fast bridge section
0,PC_WAVE,0,
0,PC_ENV_VOL,170,//180,
0,PC_ENV_SPEED,-6,//-10,
//12/3,PC_ENV_SPEED,2,
0,PC_TREMOLO_RATE,24,
0,PC_TREMOLO_LEVEL,128,
//118/2,PC_NOTE_HOLD,
118/3,PC_ENV_SPEED,-40,
30/2,PATCH_END
  };

const char patch12[] PROGMEM ={//lead short(from very low, to full in 12 cycles, then slowly dropping for 118 frames( then a bump up for 1 frame, light Tremolo?) to sustain,4 cycles from sustain to silence)
0,PC_WAVE,0,
0,PC_ENV_VOL,170,//180,
0,PC_ENV_SPEED,-6,//-10,
//12/3,PC_ENV_SPEED,2,
0,PC_TREMOLO_RATE,24,
0,PC_TREMOLO_LEVEL,64,
//118/2,PC_NOTE_HOLD,
118/3,PC_ENV_SPEED,-40,
30/2,PATCH_END
  };


const char patch13[] PROGMEM ={//lead long
0,PC_WAVE,0,
0,PC_ENV_VOL,210,
0,PC_ENV_SPEED,-4,
//12/3,PC_ENV_SPEED,2,
0,PC_TREMOLO_RATE,24,
0,PC_TREMOLO_LEVEL,64,
//118/2,PC_NOTE_HOLD,
131/3,PC_ENV_SPEED,-50,
30/2,PATCH_END
  };


const char patch14[] PROGMEM ={//lead modulated
0,PC_WAVE,1,
0,PC_ENV_VOL,200,//180,
0,PC_ENV_SPEED,-8,//-10,
//12/3,PC_ENV_SPEED,2,

0,PC_TREMOLO_RATE,24,
0,PC_TREMOLO_LEVEL,64,
//118/2,PC_NOTE_HOLD,
0,PC_LOOP_START,118/6,
1,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
0,PC_LOOP_END,2,
0,PC_ENV_SPEED,-50,
30/2,PATCH_END
};

/*
const char patch15[] PROGMEM ={//lead down bend(44MS before, 8192-1 in 241MS)
//used for notes at:
//0:20:975,0:28:780,
};
*/
/*
const char patch16[] PROGMEM ={//lead down bend(244MS before, 8192-1 in 241MS)
//used for notes at:
//36:585,44:390,
};
*/
/*
const char patch17[] PROGMEM ={//lead down bend(223MS before, 8192-410 in 39MS)
//used for notes at:
//1:18:780,3:23:658,
};
*/
/*
const char patch18[] PROGMEM ={//lead down bend(102MS before, 8192-4300 in 39MS(also as 102MS before,8192 to 4096 in 46MS))
//used for notes at:
//1:19:999,(1:27:804),((3:24:877)),
};
*/
/*
const char patch19[] PROGMEM ={//lead down bend(463MS before, 8192-1 in 485MS)
//used for notes at:
//1:23:902,1:31:707,
};
*/
/*
const char patch20[] PROGMEM ={//lead down bend(162MS before, 8192-410 in 100MS)
//used for notes at:
//1:26:585,
};
*/
/*
const char patch21[] PROGMEM ={//lead up bend(244MS before, 8192-16383 in 40MS(also as 222MS before, 8192-16383 in 41MS)
//used for notes at:
//1:35:365,(1:39:268),((1:43:170)),((1:47:072)),(3:44:145),(3:49:999),
};
*/
/*
const char patch22[] PROGMEM ={//lead up bend(945MS before, 8192-16383 in 61MS)
//used for notes at:
//2:28:292,2:32,194,2:36:097,2:43:902,
};
*/
/*
const char patch23[] PROGMEM ={//lead up bend(457MS before, 8192-16383 in 61MS)
//used for notes at:
//2:38:536,
};
*/
/*
const char patch24[] PROGMEM ={//lead up bend(945MS before, 8192-1 in 61MS(also as 945MS before, 8192-274 in 57MS)
//used for notes at:
//2:39:999,(2:51:707),((2:55:609)),
};
*/
/*
const char patch25[] PROGMEM ={//lead up bend(1463MS before, 8192-1 in 486MS)
//used for notes at:
//3:28:780,
};
*/
/*
const char patch26[] PROGMEM ={//lead up bend(123MS before, 8192-16383 in 41MS
//used for notes at:
//3:53:902,
};
*/
/*
const char patch27[] PROGMEM ={//lead bend spare
0,PC_NOTE_CUT,0,
0,PATCH_END
};
*/


const char patch28[] PROGMEM ={//bass guitar normal(bass-envelope.raw 8 frames from attack to sustain, 1 frame release to silence)
0,PC_WAVE,4,//4
0,PC_ENV_SPEED,23-8,
//0,PC_ENV_VOL,180,
//0,PC_TREMOLO_RATE,24,
//0,PC_TREMOLO_LEVEL,151,
2,PC_ENV_SPEED,0,

0,PC_NOTE_HOLD,0,

0,PC_ENV_SPEED,-8,
2,PC_NOTE_CUT,0,
0,PATCH_END

  };

const char patch29[] PROGMEM ={//rythm depth1(Channel 5)
0,PC_TREMOLO_RATE,48,//68,//78
0,PC_TREMOLO_LEVEL,171,//191

//0,PC_WAVE,3,//4
0,PC_ENV_VOL,170,//216,//80

2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
0,PC_ENV_SPEED,-3,//2,-6
0,PC_ENV_SPEED,0,
//0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-6,


2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
0,PC_ENV_SPEED,-12,//3,-20
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
255,PC_NOTE_CUT,0,
0,PATCH_END

  };

const char patch30[] PROGMEM ={//rythm depth2(Channel 5)
0,PC_TREMOLO_RATE,58,//68,//78
0,PC_TREMOLO_LEVEL,191,//191

//0,PC_WAVE,3,//4
0,PC_ENV_VOL,180,//216,//80

2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
0,PC_ENV_SPEED,-3,//2,-6
0,PC_ENV_SPEED,0,
//0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-6,


2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
2,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
0,PC_ENV_SPEED,-12,//3,-20
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
1,PC_NOTE_UP,12,
2,PC_NOTE_DOWN,12,
255,PC_NOTE_CUT,0,
0,PATCH_END

  };



  const char patch31[] PROGMEM ={//lead depth1
//0,PC_WAVE,0,
//0,PC_ENV_VOL,20,
//0,PC_ENV_SPEED,20,
0,PC_ENV_VOL,130,
0,PC_ENV_SPEED,-8,
4,PC_ENV_SPEED,0,
0,PC_TREMOLO_RATE,34,
0,PC_TREMOLO_LEVEL,64,
//118/2,PC_NOTE_HOLD,
118/3,PC_ENV_SPEED,-50,
30/2,PATCH_END

/*
0,PC_ENV_VOL,232,
0,PC_ENV_SPEED,-8,
//5,PC_ENV_SPEED,-4,
5,PC_NOTE_HOLD,0,
32,PC_ENV_SPEED,-18,
90,PC_NOTE_CUT,0,//11
0,PATCH_END
 */
  };

const char patch32[] PROGMEM ={//lead depth2
0,PC_WAVE,1,
0,PC_ENV_VOL,130,//180,
0,PC_ENV_SPEED,-8,//-10,
//12/3,PC_ENV_SPEED,2,

0,PC_TREMOLO_RATE,24,
0,PC_TREMOLO_LEVEL,64,
//118/2,PC_NOTE_HOLD,
0,PC_LOOP_START,118/6,
1,PC_NOTE_UP,12,
1,PC_NOTE_DOWN,12,
0,PC_LOOP_END,2,
0,PC_ENV_SPEED,-50,
30/2,PATCH_END
  };

const char patch33[] PROGMEM ={//tom tom low(channel 2)
0,PC_WAVE,4,
0,PC_ENV_VOL,240,
0,PC_PITCH,59,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_CUT,0,
0,PATCH_END
  };

const char patch34[] PROGMEM ={//tom tom high(channel 2)
0,PC_WAVE,4,
0,PC_ENV_VOL,240,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,1,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_DOWN,2,
1,PC_NOTE_CUT,0,
0,PATCH_END
  };
/////////////////////////////////////////////////////////

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
//0,PC_ENV_VOL,127,
3,PC_ENV_SPEED,-10,
0, PC_NOTE_CUT, 0,
0,PATCH_END
};

const char boing[] PROGMEM ={
0,PC_WAVE,8,
0,PC_ENV_VOL,0xE0,
0,PC_ENV_SPEED,-20,
//0,PC_PITCH,80,
2,PC_NOTE_DOWN,6,
2,PC_NOTE_DOWN,6,
2,PC_NOTE_CUT,0,
0,PATCH_END
};

#define IKD_MASTER_VOL	232
#define SFX_FIRE		35+0
#define SFX_EXPLODE	35+1
#define SFX_ENGINE	35+2
#define SFX_BOING	35+3

/////////////////////////////////////////////////////////
extern const char waves[];
const struct PatchStruct patches[] PROGMEM = {
{1,NULL,patch00,0,0},//hats
{1,NULL,patch01,0,0},//snare
{1,NULL,patch02,0,0},//bass
{2,waves+(256*1),patch03,0,256},//tom tom low(channel 5)
{2,waves+(256*1),patch04,0,256},//tom tom high(channel 5)
{1,NULL,patch05,0,0},//crash 1
{1,NULL,patch06,0,0},//crash 2
{0,NULL,patch07,0,0},//bass
{0,NULL,patch08,0,0},//bass spare
{0,NULL,patch09,0,0},//rythm modulated accent
{0,NULL,patch10,0,0},//rythm main
{0,NULL,patch11,0,0},//rythm spare
{0,NULL,patch12,0,0},//lead short
{0,NULL,patch13,0,0},//lead long
{0,NULL,patch14,0,0},//lead spare
{0,NULL,patch12,0,0},//lead bend 1
{0,NULL,patch12,0,0},//lead bend 2
{0,NULL,patch12,0,0},//lead bend 3
{0,NULL,patch12,0,0},//lead bend 4
{0,NULL,patch12,0,0},//lead bend 5
{0,NULL,patch12,0,0},//lead bend 6
{0,NULL,patch12,0,0},//lead bend 7
{0,NULL,patch12,0,0},//lead bend 8
{0,NULL,patch12,0,0},//lead bend 9
{0,NULL,patch12,0,0},//lead bend 10
{0,NULL,patch12,0,0},//lead bend 11
{0,NULL,patch12,0,0},//lead bend 12
{0,NULL,patch12,0,0},//lead bend spare
{2,waves+(256*4),patch28,0,256},//bass depth(Channel 5)
{2,waves+(256*3),patch29,0,256},//rythm depth1(Channel 5)
{2,waves+(256*3),patch30,0,256},//rythm depth2(Channel 5)
{2,waves+(256*0),patch31,0,256},//lead depth1(Channel 5)
{2,waves+(256*0),patch32,0,256},//lead depth2(Channel 5)
{0,NULL,patch33,0,0},//tom tom low(channel 2)
{0,NULL,patch34,0,0},//tom tom high(channel 2)
//SFX
{1, NULL, fire, 0, 0},
{1, NULL, explode, 0, 0},
{0, NULL, engine, 0, 0},
{0, NULL, boing, 0, 0},
};
