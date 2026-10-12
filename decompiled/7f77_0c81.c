
void __cdecl16far FUN_7f77_0c81(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  
  if (*DAT_451b_1d82 < 8) {
    iVar2 = *DAT_451b_1d82;
  }
  else {
    iVar2 = 7;
  }
  for (iVar3 = 0; uVar4 = (undefined2)((ulong)DAT_451b_1d82 >> 0x10), iVar3 < 7; iVar3 = iVar3 + 1)
  {
    piVar1 = (int *)DAT_451b_1d82 + iVar3;
    *piVar1 = *piVar1 - iVar2;
  }
  DAT_4125_2a49 = ((int *)DAT_451b_1d82)[DAT_451b_1d86];
  FUN_7f77_0996(1);
  FUN_7f77_0d45();
  FUN_7f77_0a86();
  return;
}


