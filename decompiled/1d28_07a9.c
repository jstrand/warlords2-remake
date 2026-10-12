
void __cdecl16far mem_unlock(undefined4 param_1)

{
  uint uVar1;
  int iVar2;
  
  iVar2 = (int)((ulong)param_1 >> 0x10);
  uVar1 = (uint)param_1;
  if ((DAT_4125_3274 == iVar2) && (uVar1 < DAT_4125_3278)) {
    if ((*(byte *)(uVar1 + 4) & 8) == 0) {
      ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] =
           ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0xdf;
      *(byte *)(uVar1 + 4) = *(byte *)(uVar1 + 4) & 0xdf;
    }
  }
  else {
    DAT_4125_3280 = 4;
    if (DAT_4125_3284 != (code *)0x0 || DAT_4125_3286 != 0) {
      (*DAT_4125_3284)(0x1d28,0,0);
    }
  }
  return;
}


