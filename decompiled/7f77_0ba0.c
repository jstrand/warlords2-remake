
void __cdecl16far FUN_7f77_0ba0(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  
  iVar2 = 7;
  while (iVar3 = iVar2 + -1, iVar4 = (int)DAT_451b_1d82,
        uVar5 = (undefined2)((ulong)DAT_451b_1d82 >> 0x10), iVar2 != 0) {
    if (iVar3 < 1) {
      piVar1 = (int *)(iVar4 + iVar3 * 2);
      *piVar1 = *piVar1 + -1;
      iVar2 = iVar3;
    }
    else {
      *(undefined2 *)(iVar4 + iVar3 * 2) = *(undefined2 *)(iVar4 + (iVar2 + -2) * 2);
      iVar2 = iVar3;
    }
  }
  DAT_451b_1d86 = DAT_451b_1d86 + 1;
  if (6 < DAT_451b_1d86) {
    DAT_451b_1d86 = 6;
    DAT_4125_2a49 = *(undefined2 *)(iVar4 + 0xc);
    FUN_7f77_0996(1);
  }
  FUN_7f77_0d45();
  FUN_7f77_0a86();
  return;
}


