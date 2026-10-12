
void __cdecl16far FUN_740d_1486(undefined4 param_1,int param_2,int param_3)

{
  int iVar1;
  
  iVar1 = (param_2 + 4 >> 3) * 8;
  if (iVar1 < 0x79) {
    iVar1 = 0x78;
  }
  if (0x207 < iVar1) {
    iVar1 = 0x208;
  }
  if (param_3 < 0x41) {
    param_3 = 0x40;
  }
  if (0x19d < param_3) {
    param_3 = 0x19e;
  }
  FUN_216d_002e(param_1,iVar1 + -0x78,param_3 + -0x40,0xf0,0x80);
  return;
}


