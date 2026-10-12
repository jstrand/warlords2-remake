
void __cdecl16far
FUN_5c52_005a(int param_1,undefined2 param_2,char *param_3,undefined2 param_4,undefined2 param_5)

{
  int iVar1;
  
  iVar1 = 0;
  do {
    if (*param_3 == '\0') {
      return;
    }
    FUN_5c52_0144(param_1,param_2,(int)*param_3,param_4,param_5);
    param_3 = (char *)CONCAT22(param_3._2_2_,(char *)param_3 + 1);
    param_1 = param_1 + 1;
    iVar1 = iVar1 + 1;
  } while (iVar1 != 0x50);
  return;
}


