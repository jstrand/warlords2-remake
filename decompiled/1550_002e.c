
undefined2 __cdecl16far
FUN_1550_002e(undefined2 *param_1,undefined2 *param_2,int param_3,undefined2 param_4,int param_5,
             int param_6)

{
  undefined2 in_AX;
  undefined2 *puVar1;
  undefined2 *puVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  puVar1 = (undefined2 *)param_1;
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  puVar2 = (undefined2 *)param_2;
  do {
    *puVar2 = *puVar1;
    puVar2[1] = puVar1[1];
    *(undefined1 *)(puVar2 + 2) = *(undefined1 *)(puVar1 + 2);
    puVar1 = (undefined2 *)((int)puVar1 + param_5 + 5);
    puVar2 = (undefined2 *)((int)puVar2 + param_6 + 5);
    param_3 = param_3 + -1;
  } while (param_3 != 0);
  return in_AX;
}


