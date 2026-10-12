
void __cdecl16far FUN_5f19_06a0(int param_1)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  
  iVar3 = *(int *)(current_player * 2 + 0xc0);
  uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar4 = (int)DAT_451b_018e;
  iVar2 = *(int *)(iVar4 + 2) * *(int *)(iVar4 + 0x2c);
  if (*(int *)(current_player * 2 + 0xd0) == 0) {
    iVar2 = iVar2 + *(int *)(iVar4 + 0x2e);
  }
  else if (iVar3 == 2) {
    iVar2 = iVar2 + *(int *)(iVar4 + 0x30);
  }
  else if (iVar3 == 1) {
    iVar2 = iVar2 + *(int *)(iVar4 + 0x32);
  }
  else if (iVar3 == 0) {
    iVar2 = iVar2 + *(int *)(iVar4 + 0x34);
  }
  *(undefined2 *)(iVar4 + param_1 * 0x5c + 0x2a6) = 0;
  if (*(int *)((int)DAT_451b_018e + 0x26) != 0) {
    iVar3 = dice(1,1000,0);
    uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (iVar3 < *(int *)((int)DAT_451b_018e + 0x26) + iVar2) {
      puVar1 = (uint *)((int)DAT_451b_018e + param_1 * 0x5c + 0x2a6);
      *puVar1 = *puVar1 | 1;
      return;
    }
  }
  uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  if (*(int *)(current_player * 0x14 + 0x185) < 100) {
    iVar2 = iVar2 + *(int *)((int)DAT_451b_018e + 0x36);
  }
  if (*(int *)((int)DAT_451b_018e + 0x28) != 0) {
    iVar3 = dice(1,1000,0);
    uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (iVar3 < *(int *)((int)DAT_451b_018e + 0x28) + iVar2) {
      puVar1 = (uint *)((int)DAT_451b_018e + param_1 * 0x5c + 0x2a6);
      *puVar1 = *puVar1 | 2;
      return;
    }
  }
  if (*(int *)((int)DAT_451b_018e + 0x2a) != 0) {
    iVar3 = dice(1,1000,0);
    uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (iVar3 < *(int *)((int)DAT_451b_018e + 0x2a) + iVar2) {
      puVar1 = (uint *)((int)DAT_451b_018e + param_1 * 0x5c + 0x2a6);
      *puVar1 = *puVar1 | 4;
    }
  }
  return;
}


