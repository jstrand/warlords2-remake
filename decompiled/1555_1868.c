
void __cdecl16far FUN_1555_1868(undefined2 *param_1,undefined4 param_2)

{
  undefined2 *puVar1;
  undefined2 uVar2;
  
  *param_1 = *DAT_451b_1f02;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  puVar1 = (undefined2 *)param_1;
  puVar1[1] = ((undefined2 *)DAT_451b_1f02)[1];
  puVar1[2] = ((undefined2 *)DAT_451b_1f02)[9];
  puVar1[3] = ((undefined2 *)DAT_451b_1f02)[10];
  *(undefined4 *)(puVar1 + 6) = param_2;
  puVar1[5] = DAT_451b_1e9c;
  puVar1[4] = DAT_451b_1e9a;
  return;
}


