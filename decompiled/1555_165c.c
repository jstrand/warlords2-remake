
int __cdecl16far FUN_1555_165c(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  int local_4;
  
  uVar4 = (undefined2)((ulong)DAT_451b_0012 >> 0x10);
  iVar2 = (int)DAT_451b_0012 + param_3 * 0xd6;
  local_4 = *(int *)(iVar2 + 6);
  iVar2 = *(int *)(iVar2 + 8);
  iVar3 = 0;
  do {
    if (*(char *)((int)DAT_451b_0012 + param_3 * 0xd6 + iVar3 + 0xe) == -1) {
      return -1;
    }
    if ((local_4 == param_1) && (iVar2 == param_2)) {
      return iVar3;
    }
    iVar1 = FUN_1a8b_0b4d(&local_4);
    if (iVar1 == 0) {
      return -1;
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 < 200);
  return -1;
}


