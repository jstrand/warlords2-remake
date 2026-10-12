
undefined2 __cdecl16far
FUN_21e2_09ea(undefined2 param_1,undefined2 param_2,char *param_3,int param_4)

{
  char *pcVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  while (pcVar1 = param_3, *param_3 != '\0') {
    iVar2 = FUN_21e2_0a53((int)*param_3);
    iVar3 = iVar3 + iVar2;
    if (param_4 <= iVar3) break;
    param_3 = (char *)CONCAT22(param_3._2_2_,(char *)param_3 + 1);
    param_1 = FUN_21e2_01f7(param_1,param_2,(int)*pcVar1,0);
  }
  if (iVar3 < param_4) {
    param_1 = FUN_21e2_0b33(param_1,param_2,param_4 - iVar3);
  }
  return param_1;
}


