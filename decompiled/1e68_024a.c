
undefined2 __cdecl16far
FUN_1e68_024a(undefined1 *param_1,undefined1 *param_2,int param_3,int param_4,int param_5,
             int param_6)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined2 in_AX;
  int iVar3;
  undefined1 *puVar4;
  undefined1 *puVar5;
  
  puVar4 = (undefined1 *)param_1;
  puVar5 = (undefined1 *)param_2;
  iVar3 = param_4;
  do {
    for (; iVar3 != 0; iVar3 = iVar3 + -1) {
      puVar2 = puVar5;
      puVar5 = puVar5 + 1;
      puVar1 = puVar4;
      puVar4 = puVar4 + 1;
      *puVar2 = *puVar1;
    }
    puVar4 = puVar4 + param_5;
    puVar5 = puVar5 + param_6;
    param_3 = param_3 + -1;
    iVar3 = param_4;
  } while (param_3 != 0);
  return in_AX;
}


