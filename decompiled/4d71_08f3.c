
void __cdecl16far FUN_4d71_08f3(undefined4 param_1)

{
  int iVar1;
  undefined4 local_6;
  
  iVar1 = dice(1,5,-10);
  local_6 = CONCAT22(local_6._2_2_,(int)param_1 + iVar1);
  iVar1 = dice(1,5,-10);
  local_6 = CONCAT22(param_1._2_2_ + iVar1,(undefined2)local_6);
  FUN_4bed_0470(&local_6);
  iVar1 = dice(1,2,1);
  FUN_4d71_09a7(param_1,local_6,iVar1);
  return;
}


