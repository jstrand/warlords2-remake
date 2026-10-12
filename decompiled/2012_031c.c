
undefined2 __cdecl16far
FUN_2012_031c(undefined4 param_1,undefined2 param_2,int param_3,undefined2 param_4,int param_5,
             undefined4 param_6,undefined2 param_7,int param_8)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  if (param_3 < *(int *)(iVar2 + 6)) {
    if (param_3 + param_5 <= *(int *)(iVar2 + 6)) {
      return 0;
    }
    iVar1 = *(int *)(iVar2 + 6) - param_3;
    (*DAT_451b_2a74)(0x2012,iVar2,uVar3,param_2,param_3,param_4,iVar1,param_6,param_7,param_8);
    param_8 = param_8 + iVar1;
    param_5 = param_5 - iVar1;
    param_3 = 0;
  }
  else {
    param_3 = param_3 - *(int *)(iVar2 + 6);
  }
  (*DAT_451b_2a74)(0x2012,*(undefined4 *)(iVar2 + 0x37),param_2,param_3,param_4,param_5,param_6,
                   param_7,param_8);
  return 1;
}


