
void __cdecl16near __crtinit(byte param_1)

{
  undefined2 uVar1;
  int iVar2;
  
  DAT_4125_3e78 = param_1;
  uVar1 = __video_int();
  DAT_4125_3e7a = (char)((uint)uVar1 >> 8);
  if ((byte)uVar1 != DAT_4125_3e78) {
    __video_int();
    uVar1 = __video_int();
    DAT_4125_3e78 = (byte)uVar1;
    DAT_4125_3e7a = (char)((uint)uVar1 >> 8);
  }
  if (((DAT_4125_3e78 < 4) || (0x3f < DAT_4125_3e78)) || (DAT_4125_3e78 == 7)) {
    DAT_4125_3e7b = 0;
  }
  else {
    DAT_4125_3e7b = 1;
  }
  if (DAT_4125_3e78 == 0x40) {
    DAT_4125_3e79 = DAT_0000_0484 + '\x01';
  }
  else {
    DAT_4125_3e79 = '\x19';
  }
  if (DAT_4125_3e78 != 7) {
    iVar2 = __rom_strcmp(0x3e83,0x4125,0xffea,0xf000);
    if (iVar2 == 0) {
      iVar2 = __ega_installed();
      if (iVar2 == 0) {
        DAT_4125_3e7c = 1;
        goto LAB_1000_3142;
      }
    }
  }
  DAT_4125_3e7c = 0;
LAB_1000_3142:
  if (DAT_4125_3e78 == 7) {
    DAT_4125_3e7f = 0xb000;
  }
  else {
    DAT_4125_3e7f = 0xb800;
  }
  DAT_4125_3e7d = 0;
  DAT_4125_3e73 = 0;
  DAT_4125_3e72 = 0;
  DAT_4125_3e74 = DAT_4125_3e7a + -1;
  DAT_4125_3e75 = DAT_4125_3e79 + -1;
  return;
}


