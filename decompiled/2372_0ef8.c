
void __cdecl16far FUN_2372_0ef8(int param_1,int param_2)

{
  ulong *puVar1;
  int iVar2;
  ulong uVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  byte local_8;
  
  iVar6 = ((int *)DAT_451b_2fc8)[5];
  while (iVar7 = iVar6 + -1, iVar6 != 0) {
    uVar8 = (undefined2)((ulong)DAT_451b_2fc8 >> 0x10);
    piVar4 = (int *)DAT_451b_2fc8;
    iVar6 = iVar7;
    if (*(int *)((int)*(undefined4 *)(piVar4 + 2) + iVar7 * 8) == param_1) {
      iVar2 = piVar4[1];
      iVar5 = *DAT_451b_2fc8 + *(int *)((int)*(undefined4 *)(piVar4 + 2) + iVar7 * 8 + 2) * 0x2e;
      local_8 = (byte)*(undefined2 *)((int)*(undefined4 *)(piVar4 + 2) + iVar7 * 8 + 4);
      uVar3 = 1LL << (local_8 & 0x1f);
      puVar1 = (ulong *)(iVar5 + 0x1a);
      *puVar1 = *puVar1 & (uVar3 ^ 0xffffff);
      if (param_2 != 0) {
        puVar1 = (ulong *)(iVar5 + 0x1a);
        *puVar1 = *puVar1 | uVar3;
      }
    }
  }
  return;
}


