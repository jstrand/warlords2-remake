
undefined2 __cdecl16far FUN_1d28_01d5(int param_1)

{
  code *pcVar1;
  undefined2 unaff_ES;
  bool bVar2;
  bool bVar3;
  
  if (param_1 != 0) {
    bVar2 = false;
    bVar3 = DAT_4125_3274 == 0;
    if (bVar3) {
      pcVar1 = (code *)swi(0x21);
      (*pcVar1)();
      if (!bVar2) {
        return 0;
      }
    }
    else {
      FUN_1d28_0244();
      if (!bVar3) {
        ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0] = '\0';
        ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[1] = '\0';
        ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] =
             ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] | 0x20;
        return 0;
      }
    }
    DAT_4125_3280 = 2;
  }
  return 0;
}


