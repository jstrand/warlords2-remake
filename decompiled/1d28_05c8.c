
void __cdecl16near FUN_1d28_05c8(void)

{
  byte *pbVar1;
  int iVar2;
  
  DAT_4125_327c = DAT_4125_327a;
  iVar2 = DAT_4125_327a;
  do {
    while (((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x40U) != 0 &&
           ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x20U) == 0))) {
      pbVar1 = (byte *)(*(int *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 4);
      *pbVar1 = *pbVar1 | 0x10;
      FUN_1d28_060d();
    }
    iVar2 = iVar2 + *(int *)&DAT_4125_0002;
  } while (iVar2 != DAT_4125_327e);
  return;
}


