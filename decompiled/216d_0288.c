
void __cdecl16far FUN_216d_0288(undefined2 *param_1)

{
  int iVar1;
  
  if (DAT_4125_3384 == 0) {
    for (iVar1 = ((undefined2 *)param_1)[1];
        iVar1 < (int)(((undefined2 *)param_1)[1] + ((undefined2 *)param_1)[3]); iVar1 = iVar1 + 1) {
      FUN_2133_02fe(*param_1,iVar1,((undefined2 *)param_1)[2]);
    }
  }
  else {
    FUN_216d_02d8((undefined2 *)param_1,param_1._2_2_);
  }
  return;
}


