
void __cdecl16far FUN_216d_00da(int *param_1,int *param_2,undefined2 param_3,undefined2 param_4)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  int *piVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  
  piVar3 = (int *)param_2;
  uVar6 = (undefined2)((ulong)param_2 >> 0x10);
  piVar5 = (int *)param_1;
  uVar8 = (undefined2)((ulong)param_1 >> 0x10);
  if (*param_1 < *param_2) {
    iVar2 = *param_1;
    piVar4 = piVar3;
    uVar7 = uVar6;
  }
  else {
    iVar2 = *param_2;
    piVar4 = piVar5;
    uVar7 = uVar8;
  }
  if (piVar5[1] < piVar3[1]) {
    iVar1 = piVar5[1];
    piVar5 = piVar3;
    uVar8 = uVar6;
  }
  else {
    iVar1 = piVar3[1];
  }
  FUN_216d_002e(param_3,param_4,iVar2,iVar1,*piVar4,piVar5[1]);
  return;
}


