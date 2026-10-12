
void __cdecl16far FUN_563e_1251(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  undefined2 local_12 [4];
  undefined2 local_a [4];
  
  iVar2 = 4;
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    local_12[iVar3] = 0xffff;
    local_a[iVar3] = 0xffff;
    uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar4 = (int)DAT_451b_018e;
    iVar2 = iVar3;
    if (*(int *)(iVar4 + param_1 * 0x5c + iVar3 * 2 + 0x28a) != -1) {
      iVar1 = *(int *)(iVar4 + param_1 * 0x5c + iVar3 * 2 + 0x28a);
      if ((*(char *)(iVar1 * 0x16 + -0x777b) == current_player) &&
         (param_1 + 1U == (*(byte *)(iVar1 * 0x16 + -0x7772) & 0x70) >> 4)) {
        local_a[iVar3] = *(undefined2 *)(iVar1 * 0x16 + -0x7780);
        local_12[iVar3] = *(undefined2 *)(iVar1 * 0x16 + -0x777e);
      }
      else {
        *(undefined2 *)(iVar4 + param_1 * 0x5c + iVar3 * 2 + 0x28a) = 0xffff;
      }
    }
  }
  return;
}


