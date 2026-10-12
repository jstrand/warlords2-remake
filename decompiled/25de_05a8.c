
void __cdecl16near FUN_25de_05a8(void)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  undefined2 unaff_ES;
  undefined1 uVar4;
  
  DAT_4125_011c = DAT_4125_011c + 1;
  if (*(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0xc) == 0) {
    uVar4 = 0;
    ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x16] =
         ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x16] | 8;
    iVar3 = FUN_25de_055e();
    *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 10) = unaff_ES;
    (**(code **)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x14))();
    if ((bool)uVar4) {
      __ovr_error();
      return;
    }
    FUN_25de_0739();
  }
  else {
    ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x17] = '\x01';
    ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x16] =
         ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x16] | 4;
  }
  FUN_25de_0676();
  ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x17] =
       ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x17] +
       (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x16] & 3U);
  uVar2 = FUN_25de_0789();
  iVar3 = DAT_4125_012c;
  while ((iVar1 = *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x18), iVar1 != 0
         && (uVar2 < DAT_4125_0118))) {
    if (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x17] == '\0') {
      FUN_25de_06b5();
      iVar3 = FUN_25de_07a5();
    }
    else {
      iVar3 = 0;
    }
    uVar2 = iVar3 + uVar2;
    iVar3 = iVar1;
  }
  return;
}


