
void __cdecl16far FUN_18a9_0273(int param_1,int *param_2)

{
  undefined2 uVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int *piVar5;
  undefined2 uVar6;
  
  uVar6 = (undefined2)((ulong)DAT_451b_0ce0 >> 0x10);
  iVar4 = (int)DAT_451b_0ce0;
  iVar2 = *(int *)*(undefined4 *)(iVar4 + 0xc);
  do {
    if (iVar2 == 0) {
      return;
    }
    uVar1 = *(undefined2 *)(iVar4 + 6);
    piVar3 = (int *)(*(int *)(iVar4 + 4) + (iVar2 + -1) * 0xe);
    iVar2 = iVar2 + -1;
  } while (*piVar3 != param_1);
  piVar3[2] = *param_2;
  uVar6 = (undefined2)((ulong)param_2 >> 0x10);
  piVar5 = (int *)param_2;
  piVar3[3] = piVar5[1];
  piVar3[4] = piVar5[2];
  piVar3[5] = piVar5[3];
  return;
}


