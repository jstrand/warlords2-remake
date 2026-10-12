
void __cdecl16near FUN_25de_063b(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = 0;
  do {
    iVar3 = DAT_4125_012c;
    iVar1 = iVar1 + 1;
    DAT_4125_012c = *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x18);
  } while (DAT_4125_012c != 0);
  DAT_4125_0120 = DAT_4125_0126;
  do {
    iVar2 = iVar1;
    *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x18) = DAT_4125_012c;
    DAT_4125_012c = iVar3;
    iVar1 = FUN_25de_07a5();
    DAT_4125_0120 = DAT_4125_0120 - iVar1;
    FUN_25de_06e8();
    iVar1 = iVar2 + -1;
    iVar3 = iVar2;
  } while (iVar1 != 0);
  DAT_4125_0120 = DAT_4125_0124;
  return;
}


