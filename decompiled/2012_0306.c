
void __cdecl16far FUN_2012_0306(undefined4 param_1,undefined4 param_2)

{
  uint *puVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  puVar1 = (uint *)((int)param_1 + 0x35);
  *puVar1 = *puVar1 | 1;
  *(undefined4 *)((int)param_1 + 0x37) = param_2;
  return;
}


