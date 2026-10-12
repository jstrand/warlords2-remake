
void __cdecl16far FUN_18a9_07ed(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined2 uVar4;
  
  uVar4 = (undefined2)((ulong)DAT_451b_0ce0 >> 0x10);
  iVar3 = (int)DAT_451b_0ce0;
  iVar1 = *(int *)*(undefined4 *)(iVar3 + 0xc);
  do {
    if (iVar1 == 0) {
      return;
    }
    piVar2 = (int *)(*(int *)(iVar3 + 4) + (iVar1 + -1) * 0xe);
    iVar1 = iVar1 + -1;
  } while (*piVar2 != param_1);
  piVar2[1] = 1;
  return;
}


