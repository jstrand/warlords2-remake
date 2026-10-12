
undefined2 __cdecl16far FUN_5f19_07f8(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  
  uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar3 = (int)DAT_451b_018e;
  iVar1 = *(int *)(iVar3 + 0x24a);
  do {
    iVar2 = iVar1 + -1;
    if (iVar1 == 0) {
      return 0;
    }
    iVar1 = iVar2;
  } while ((*(int *)(iVar3 + iVar2 * 0x5c + 0x24c) == 0) ||
          (*(int *)(iVar3 + iVar2 * 0x5c + 0x250) != param_1));
  return 1;
}


