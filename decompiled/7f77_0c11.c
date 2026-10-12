
void __cdecl16far FUN_7f77_0c11(void)

{
  int iVar1;
  undefined2 *puVar2;
  undefined2 uVar3;
  
  for (iVar1 = 0; puVar2 = (undefined2 *)DAT_451b_1d82,
      uVar3 = (undefined2)((ulong)DAT_451b_1d82 >> 0x10), iVar1 < 7; iVar1 = iVar1 + 1) {
    if (iVar1 < 6) {
      puVar2[iVar1] = puVar2[iVar1 + 1];
    }
    else {
      puVar2[iVar1] = puVar2[iVar1] + 1;
    }
  }
  DAT_451b_1d86 = DAT_451b_1d86 + -1;
  if (DAT_451b_1d86 < 0) {
    DAT_451b_1d86 = 0;
    DAT_4125_2a49 = *DAT_451b_1d82;
    FUN_7f77_0996(1);
  }
  FUN_7f77_0d45();
  FUN_7f77_0a86();
  return;
}


