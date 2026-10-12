
undefined2 __cdecl16far FUN_558d_0a6e(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  
  uVar1 = 0;
  if (((opt_diplomacy != 0) && (*(int *)((int)DAT_451b_018e + 0x38) != 0)) &&
     (*(int *)(current_player * 2 + 0xd0) != 0)) {
    iVar2 = 8;
    while (iVar3 = iVar2 + -1, iVar2 != 0) {
      iVar2 = iVar3;
      if (((*(int *)(iVar3 * 2 + 0x137) != 0) && (*(int *)(iVar3 * 2 + 0xd0) == 0)) &&
         (((*(byte *)(param_1 * 8 + iVar3 + 0x153b) & 3) == 2 && (*(int *)(iVar3 * 2 + 0x40f2) != 0)
          ))) {
        uVar1 = 1;
      }
    }
  }
  return uVar1;
}


