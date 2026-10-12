
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far FUN_1ea3_0000(void)

{
  code *pcVar1;
  
  DAT_4125_3374 = _DAT_0000_0463;
  pcVar1 = (code *)swi(0x12);
  DAT_4125_3376 = (*pcVar1)();
  if (DAT_4125_3376 < 0xf0) {
    return 0xffff;
  }
  if (DAT_4125_3376 < 0x101) {
    DAT_4125_3378 = 1;
  }
  else if (DAT_4125_3376 < 0x181) {
    DAT_4125_3378 = 3;
  }
  else if (DAT_4125_3376 < 0x201) {
    DAT_4125_3378 = 5;
  }
  else if (DAT_4125_3376 < 0x281) {
    DAT_4125_3378 = 7;
  }
  else {
    DAT_4125_3378 = 9;
  }
  return DAT_4125_3378;
}


