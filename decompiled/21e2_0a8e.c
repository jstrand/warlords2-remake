
int __cdecl16far FUN_21e2_0a8e(char *param_1)

{
  char *pcVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  while (pcVar1 = param_1, *param_1 != '\0') {
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
    iVar2 = FUN_21e2_0a53((int)*pcVar1);
    iVar3 = iVar3 + iVar2;
  }
  return iVar3;
}


