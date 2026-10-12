
void __cdecl16far FUN_2012_09e3(undefined4 param_1,int param_2)

{
  uint *puVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  puVar1 = (uint *)((int)param_1 + 0x35);
  *puVar1 = *puVar1 & 0xfff7;
  if (param_2 != 0) {
    puVar1 = (uint *)((int)param_1 + 0x35);
    *puVar1 = *puVar1 | 8;
  }
  return;
}


