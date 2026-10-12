
void __cdecl16far
FUN_7b4c_01ad(undefined4 param_1,long param_2,long param_3,long param_4,long param_5)

{
  uint uVar1;
  
  FUN_22bf_07a5();
  FUN_54f6_0000();
  FUN_78a8_06ae(0xf0001,0x30000);
  FUN_7ecb_00d6(DAT_4125_2368,param_1);
  uVar1 = (uint)(param_2 != 0);
  if (param_3 != 0) {
    uVar1 = uVar1 + 1;
  }
  if (param_4 != 0) {
    uVar1 = uVar1 + 1;
  }
  if (param_5 != 0) {
    uVar1 = uVar1 + 1;
  }
  FUN_78a8_06ae(0xf0002,0x30000);
  if (param_2 != 0) {
    FUN_7ecb_00d6(*(undefined2 *)((char *)s_terrain_d__4125_2316 + 2 + uVar1 * 0x10),
                  *(undefined2 *)((char *)s_terrain_d__4125_2316 + 4 + uVar1 * 0x10),param_2);
  }
  if (param_3 != 0) {
    FUN_7ecb_00d6(*(undefined2 *)((char *)s_terrain_d__4125_2316 + 6 + uVar1 * 0x10),
                  *(undefined2 *)((char *)s_terrain_d__4125_2316 + 8 + uVar1 * 0x10),param_3);
  }
  if (param_4 != 0) {
    FUN_7ecb_00d6(*(undefined2 *)((char *)s_terrain_d__4125_2316 + 10 + uVar1 * 0x10),
                  *(undefined2 *)(uVar1 * 0x10 + 0x2322),param_4);
  }
  if (param_5 != 0) {
    FUN_7ecb_00d6(*(undefined2 *)(uVar1 * 0x10 + 0x2324),*(undefined2 *)(uVar1 * 0x10 + 0x2326),
                  param_5);
  }
  FUN_22bf_0757();
  return;
}


