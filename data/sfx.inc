// SFX for IKD

const char fire[] PROGMEM = {
  0, PC_ENV_VOL, 255,
  0, PC_NOISE_PARAMS, 1,
  0, PC_ENV_SPEED, -9,
  0, PATCH_END,
};
const struct PatchStruct patches[] PROGMEM = {
  {1, NULL, fire, 0, 0},
};
#define FIRE 0
