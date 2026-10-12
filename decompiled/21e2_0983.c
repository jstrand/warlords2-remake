
void __cdecl16far FUN_21e2_0983(undefined2 param_1,undefined2 param_2,int param_3,int param_4)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_SS;
  char local_8 [6];
  
  itoa(param_3,(char *)CONCAT22(unaff_SS,local_8),10);
  iVar1 = FUN_21e2_0a8e();
  if (param_4 < iVar1) {
    FUN_21e2_0b33(param_1,param_2,param_4);
  }
  else {
    uVar2 = FUN_21e2_0b33(param_1,param_2,param_4 - iVar1);
    FUN_21e2_0401(uVar2,param_2,local_8);
  }
  return;
}


