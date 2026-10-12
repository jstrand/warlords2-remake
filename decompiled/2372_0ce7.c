
void __cdecl16far FUN_2372_0ce7(int param_1,int param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  
  uVar5 = (undefined2)((ulong)DAT_451b_2fc8 >> 0x10);
  iVar3 = ((int *)DAT_451b_2fc8)[4];
  do {
    if (iVar3 == 0) {
      return;
    }
    iVar1 = ((int *)DAT_451b_2fc8)[1];
    iVar4 = *DAT_451b_2fc8 + (iVar3 + -1) * 0x2e;
    iVar3 = iVar3 + -1;
  } while (*(int *)(iVar4 + 8) != param_1);
  if (param_2 == 0) {
    if (param_3 == 0) {
      uVar2 = 0;
    }
    else {
      uVar2 = 0xffff;
    }
    *(ulong *)(iVar4 + 0x20) = (ulong)uVar2;
  }
  else {
    *(ulong *)(iVar4 + 0x20) =
         *(ulong *)(iVar4 + 0x20) & (ulong)(1 << ((byte)param_2 & 0x1f) ^ 0xffff);
    if (param_3 != 0) {
      *(ulong *)(iVar4 + 0x20) = *(ulong *)(iVar4 + 0x20) | (long)(1 << ((byte)param_2 & 0x1f));
    }
  }
  return;
}


