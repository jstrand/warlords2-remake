
void __cdecl16near FUN_25de_055e(void)

{
  char *pcVar1;
  uint uVar2;
  undefined2 unaff_ES;
  byte in_CF;
  undefined4 uVar3;
  
  DAT_4125_012a = DAT_4125_012a + 1;
  FUN_25de_07b1();
  while( true ) {
    uVar3 = FUN_25de_0789();
    uVar2 = (uint)((ulong)uVar3 >> 0x10);
    if (uVar2 <= (uint)uVar3) break;
    if ((in_CF & 1) != 0) {
      FUN_25de_063b(uVar2);
    }
    in_CF = 0;
    if (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x17] == '\0') {
      DAT_4125_012c = *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x18);
      FUN_25de_0623();
      FUN_25de_07a5();
    }
    else {
      pcVar1 = (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x17;
      DAT_4125_012c = *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x18);
      *pcVar1 = *pcVar1 + -1;
      FUN_25de_06e8();
      FUN_25de_0739();
    }
  }
  *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0xc) = DAT_4125_0120;
  return;
}


