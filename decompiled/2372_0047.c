
void __cdecl16far FUN_2372_0047(int *param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  
  iVar2 = 0xff;
  if ((*param_1 != 0x13b) && (*param_1 != 0x152)) {
    uVar5 = (undefined2)((ulong)DAT_451b_2fc8 >> 0x10);
    iVar4 = ((int *)DAT_451b_2fc8)[4];
    do {
      if (iVar4 == 0) goto LAB_2372_00b5;
      iVar1 = ((int *)DAT_451b_2fc8)[1];
      iVar3 = *DAT_451b_2fc8 + (iVar4 + -1) * 0x2e;
      iVar4 = iVar4 + -1;
    } while (*(int *)(iVar3 + 10) != *param_1);
    iVar2 = *(int *)(iVar3 + 8);
LAB_2372_00b5:
    if (iVar2 == 0xff) {
      FUN_2372_111b((int *)param_1,param_1._2_2_);
      return;
    }
    if (DAT_451b_3015 == iVar2) {
      DAT_451b_3011 = DAT_451b_3015;
      DAT_451b_3013 = DAT_451b_3017;
    }
    else {
      DAT_451b_3011 = iVar2;
      DAT_451b_3013 = FUN_2372_0c50(iVar2);
    }
  }
  iVar2 = FUN_2372_00f0(2);
  *param_1 = iVar2;
  return;
}


