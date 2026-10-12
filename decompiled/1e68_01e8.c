
undefined2 __cdecl16far
FUN_1e68_01e8(undefined4 *param_1,undefined4 *param_2,int param_3,uint param_4,int param_5,
             int param_6)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined2 in_AX;
  uint uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  
  puVar4 = (undefined4 *)param_1;
  puVar5 = (undefined4 *)param_2;
  do {
    for (uVar3 = param_4 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      puVar2 = puVar5;
      puVar5 = puVar5 + 1;
      puVar1 = puVar4;
      puVar4 = puVar4 + 1;
      *puVar2 = *puVar1;
    }
    puVar4 = (undefined4 *)((int)puVar4 + param_5);
    puVar5 = (undefined4 *)((int)puVar5 + param_6);
    param_3 = param_3 + -1;
  } while (param_3 != 0);
  return in_AX;
}


