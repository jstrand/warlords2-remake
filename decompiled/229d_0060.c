
int __cdecl16far
FUN_229d_0060(undefined2 param_1,undefined2 param_2,void *param_3,undefined2 param_4,uint param_5)

{
  int iVar1;
  undefined2 in_DX;
  undefined4 local_6;
  
  iVar1 = FUN_229d_00a9(param_1,param_2,param_5);
  local_6 = CONCAT22(in_DX,iVar1);
  if (local_6 != 0) {
    memcpy((void *)CONCAT22(*(undefined2 *)(iVar1 + 10),(void *)*(undefined2 *)(iVar1 + 8)),
           (void *)CONCAT22(param_4,param_3),param_5);
  }
  return iVar1;
}


