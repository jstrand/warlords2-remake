
void __cdecl16far FUN_563e_066d(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  
  uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  *(undefined1 *)((int)DAT_451b_018e + *(int *)((int)DAT_451b_018e + param_1 * 0x5c + 0x250) + 0x56)
       = 8;
  iVar2 = 4;
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar1 = (int)DAT_451b_018e;
    iVar2 = iVar3;
    if (*(int *)(iVar1 + param_1 * 0x5c + iVar3 * 2 + 0x252) != -1) {
      *(undefined1 *)(iVar1 + *(int *)(iVar1 + param_1 * 0x5c + iVar3 * 2 + 0x252) + 0x56) = 8;
    }
  }
  FUN_5f19_08ba(param_1);
  return;
}


