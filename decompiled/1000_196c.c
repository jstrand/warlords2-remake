
uint __pow10_near(uint param_1)

{
  if (((-0x1345 < (int)param_1) && ((int)param_1 < 0x1345)) && (param_1 != 0)) {
    if ((int)param_1 < 0) {
      param_1 = -param_1;
    }
    if (param_1 >> 4 == 0) {
      param_1 = 0;
    }
    else {
      param_1 = param_1 >> 0xd;
    }
  }
  return param_1;
}


