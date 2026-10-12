
void __cdecl16far
FUN_740d_131a(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4,int param_5,
             int param_6)

{
  int iVar1;
  
  if (param_6 != 0) {
    param_4 = (param_4 - DAT_3c04_0177) * 0x28 + 0x24;
    param_5 = (param_5 - DAT_3c04_0175) * 0x28 + 0x32;
  }
  iVar1 = (param_4 + 4 >> 3) * 8;
  if (iVar1 < 0x81) {
    iVar1 = 0x80;
  }
  if (0x1ff < iVar1) {
    iVar1 = 0x200;
  }
  if (param_5 < 0x26) {
    param_5 = 0x25;
  }
  if (0x1b8 < param_5) {
    param_5 = 0x1b9;
  }
  FUN_216d_002e(param_2,iVar1 + -0x7f,param_5 + -0x24,0xfe,0x49);
  FUN_216d_002e(param_1,iVar1 + -0x80,param_5 + -0x25,0x100,0x4b);
  FUN_216d_002e(param_3,iVar1 + -0x7b,param_5 + -0x20,0xf6,0x41);
  return;
}


