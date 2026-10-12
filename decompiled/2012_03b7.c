
undefined2 __cdecl16far
FUN_2012_03b7(undefined4 param_1,undefined2 param_2,int param_3,undefined2 param_4,int param_5,
             undefined4 param_6,undefined2 param_7,int param_8)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined4 uVar4;
  
  uVar3 = (undefined2)((ulong)param_6 >> 0x10);
  iVar2 = (int)param_6;
  if (param_8 < *(int *)(iVar2 + 6)) {
    if (param_8 + param_5 <= *(int *)(iVar2 + 6)) {
      return 0;
    }
    iVar1 = *(int *)(iVar2 + 6) - param_8;
    (*DAT_451b_2a74)(0x2012,param_1,param_2,param_3,param_4,iVar1,iVar2,uVar3,param_7,param_8);
    param_8 = 0;
    uVar4 = *(undefined4 *)(iVar2 + 0x37);
    param_5 = param_5 - iVar1;
  }
  else {
    param_8 = param_8 - *(int *)(iVar2 + 6);
    uVar4 = *(undefined4 *)(iVar2 + 0x37);
    iVar1 = param_3;
  }
  (*DAT_451b_2a74)(0x2012,param_1,param_2,iVar1,param_4,param_5,uVar4,param_7,param_8);
  return 1;
}


