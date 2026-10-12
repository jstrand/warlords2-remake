
void __cdecl16far FUN_2372_11fd(void)

{
  uint uVar1;
  
  _bios_timeofday(0,(undefined4 *)&DAT_451b_2fc0,0x4125);
  uVar1 = (int)DAT_451b_2fc0 - (int)DAT_451b_2fbc >> 0xf;
  if (2 < (int)(((int)DAT_451b_2fc0 - (int)DAT_451b_2fbc ^ uVar1) - uVar1)) {
    FUN_2372_102d(0xff);
    DAT_451b_2fbc = DAT_451b_2fc0;
  }
  return;
}


