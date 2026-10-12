
void __cdecl16far FUN_2372_0e17(int param_1,int param_2,int param_3)

{
  int iVar1;
  ulong uVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  
  uVar6 = (undefined2)((ulong)DAT_451b_2fc8 >> 0x10);
  iVar4 = ((int *)DAT_451b_2fc8)[4];
  do {
    if (iVar4 == 0) {
      return;
    }
    iVar1 = ((int *)DAT_451b_2fc8)[1];
    iVar5 = *DAT_451b_2fc8 + (iVar4 + -1) * 0x2e;
    iVar4 = iVar4 + -1;
  } while (*(int *)(iVar5 + 8) != param_1);
  uVar2 = 1LL << ((byte)param_2 & 0x1f);
  if (param_2 == 0) {
    if (param_3 == 0) {
      uVar3 = 0;
    }
    else {
      uVar3 = 0xffffff;
    }
    *(undefined4 *)(iVar5 + 0x1a) = uVar3;
  }
  else {
    *(ulong *)(iVar5 + 0x1a) = *(ulong *)(iVar5 + 0x1a) & (uVar2 ^ 0xffffff);
    if (param_3 != 0) {
      *(ulong *)(iVar5 + 0x1a) = *(ulong *)(iVar5 + 0x1a) | uVar2;
    }
  }
  return;
}


