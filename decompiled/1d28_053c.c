
void __cdecl16near FUN_1d28_053c(void)

{
  uint in_AX;
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = DAT_4125_327c;
  do {
    if (((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x80U) == 0) &&
       (in_AX <= *(uint *)&DAT_4125_0002)) {
      if ((in_AX != *(uint *)&DAT_4125_0002) && (in_AX + 1 != *(int *)&DAT_4125_0002)) {
        iVar3 = *(int *)&DAT_4125_0002 - in_AX;
        iVar1 = iVar2 + in_AX;
        FUN_1d28_068c();
        *(int *)&DAT_4125_0002 = iVar3;
        *(int *)0x0 = iVar2;
        if (iVar3 + iVar1 != DAT_4125_327e) {
          *(int *)0x0 = iVar1;
        }
        *(uint *)&DAT_4125_0002 = in_AX;
      }
      ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0] = '\0';
      ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[1] = '\0';
      ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] = -0x80;
      DAT_4125_327c = iVar2;
      return;
    }
    iVar2 = iVar2 + *(int *)&DAT_4125_0002;
    if (iVar2 == DAT_4125_327e) {
      iVar2 = DAT_4125_327a;
    }
  } while (iVar2 != DAT_4125_327c);
  return;
}


