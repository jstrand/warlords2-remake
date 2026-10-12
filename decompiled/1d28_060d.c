
void __cdecl16near FUN_1d28_060d(void)

{
  int iVar1;
  int iVar2;
  int unaff_ES;
  
  ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] = '\0';
  iVar2 = unaff_ES + *(int *)&DAT_4125_0002;
  DAT_4125_327c = unaff_ES;
  if ((iVar2 != DAT_4125_327e) &&
     ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x80U) == 0)) {
    *(int *)&DAT_4125_0002 = *(int *)&DAT_4125_0002 + *(int *)&DAT_4125_0002;
    iVar2 = unaff_ES + *(int *)&DAT_4125_0002;
    if (iVar2 != DAT_4125_327e) {
      *(int *)0x0 = unaff_ES;
    }
  }
  if (unaff_ES != DAT_4125_327a) {
    iVar2 = *(int *)0x0;
    iVar1 = unaff_ES + *(int *)&DAT_4125_0002;
    if ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x80U) == 0) {
      DAT_4125_327c = iVar2;
      *(int *)&DAT_4125_0002 = *(int *)&DAT_4125_0002 + *(int *)&DAT_4125_0002;
      if (iVar1 != DAT_4125_327e) {
        *(int *)0x0 = iVar2;
      }
    }
  }
  return;
}


