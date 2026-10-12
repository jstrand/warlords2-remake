
undefined2 __IOerror(int param_1)

{
  if (param_1 < 0) {
    if (param_1 == -0x30 || -param_1 < 0x30) {
      DAT_4125_007f = -param_1;
      DAT_4125_3d28 = 0xffff;
      return 0xffff;
    }
  }
  else if (param_1 < 0x59) goto LAB_1000_1ead;
  param_1 = 0x57;
LAB_1000_1ead:
  DAT_4125_3d28 = param_1;
  DAT_4125_007f = (int)*(char *)(param_1 + 0x3d2a);
  return 0xffff;
}


