
void __cdecl16far FUN_1d28_048e(undefined4 param_1)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
  iVar3 = (int)((ulong)param_1 >> 0x10);
  uVar2 = (uint)param_1;
  if (iVar3 == DAT_4125_3274) {
    if (((*(byte *)(uVar2 + 4) & 0x80) == 0) || (DAT_4125_3278 <= uVar2)) {
      DAT_4125_3280 = 5;
    }
    else if ((*(byte *)(uVar2 + 4) & 8) == 0) {
      iVar1 = *(int *)(uVar2 + 2);
      *(undefined1 *)(uVar2 + 4) = 0;
      if (*(uint *)s_Borland_C_____Copyright_1991_Bor_4125_0004 == uVar2) {
        FUN_1d28_060d();
      }
      else {
        DAT_4125_3280 = 6;
      }
    }
    else {
      *(undefined1 *)(uVar2 + 4) = 0;
      FUN_1d28_0bab();
    }
  }
  else {
    DAT_4125_3280 = 4;
  }
  return;
}


