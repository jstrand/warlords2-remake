
undefined2 __cdecl16far FUN_5f19_0a7a(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  
  uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar3 = (int)DAT_451b_018e;
  iVar1 = *(int *)(iVar3 + 0x24a);
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if ((*(int *)(iVar3 + iVar2 * 0x5c + 0x24c) != 0) &&
       (*(int *)(iVar3 + iVar2 * 0x5c + 0x250) == param_1)) {
      return 0;
    }
  }
  if ((*(char *)(iVar3 + param_1 + 0x56) != '\b') && (*(char *)(iVar3 + param_1 + 0x56) != '\x05'))
  {
    if (param_2 == 0) {
      return 0;
    }
    if (*(char *)(iVar3 + param_1 + 0x56) != '\x06') {
      return 0;
    }
  }
  return 1;
}


