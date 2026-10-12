
void __cdecl16far FUN_4bed_03aa(int param_1,int param_2)

{
  undefined2 *puVar1;
  undefined2 uVar2;
  
  if (DAT_4125_26c2 != 0) {
    uVar2 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    puVar1 = (undefined2 *)DAT_451b_002a;
    if ((*(byte *)(param_2 * 0x70 + param_1) & 0x1f) == 0) {
      puVar1 = (undefined2 *)DAT_451b_002a +
               *(char *)((int)(undefined2 *)DAT_451b_002a + param_1 + param_2 * 0x70 + 0x6120);
    }
    param_1 = param_1 * 2;
    param_2 = param_2 * 2;
    FUN_24d0_0002(*puVar1);
    FUN_24d0_0065(param_1 + 400,param_2 + 0x1e);
    FUN_24d0_0065(param_1 + 0x191,param_2 + 0x1e);
    FUN_24d0_0065(param_1 + 400,param_2 + 0x1f);
    FUN_24d0_0065(param_1 + 0x191,param_2 + 0x1f);
  }
  return;
}


