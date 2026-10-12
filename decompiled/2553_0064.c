
void __cdecl16far
FUN_2553_0064(undefined2 param_1,undefined2 *param_2,int param_3,undefined2 param_4,int param_5,
             int param_6,int param_7,int param_8)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  int iVar3;
  int *piVar4;
  undefined2 *puVar5;
  undefined2 unaff_SS;
  
  piVar4 = (undefined2 *)&DAT_451b_2a78 + param_6;
  do {
    puVar5 = (undefined2 *)(param_5 + *piVar4);
    for (iVar3 = param_7; iVar3 != 0; iVar3 = iVar3 + -1) {
      puVar2 = puVar5;
      puVar5 = puVar5 + 1;
      puVar1 = param_2;
      param_2 = param_2 + 1;
      *puVar2 = *puVar1;
    }
    param_2 = param_2 + (param_3 - param_7);
    piVar4 = piVar4 + 1;
    param_8 = param_8 + -1;
  } while (param_8 != 0);
  return;
}


