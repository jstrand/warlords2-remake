
void __cdecl16far FUN_7b4c_00e2(undefined4 param_1,long param_2,long param_3)

{
  int iVar1;
  
  iVar1 = 2;
  FUN_22bf_07a5();
  FUN_54f6_0000();
  FUN_78a8_06ae(0xf0001,0x30000);
  FUN_7ecb_00d6(DAT_4125_2368,param_1);
  if (param_2 != 0) {
    iVar1 = 3;
  }
  if (param_3 != 0) {
    iVar1 = iVar1 + 1;
  }
  FUN_78a8_06ae(0xf0002,0x30000);
  if (param_2 != 0) {
    FUN_7ecb_00d6(*(undefined2 *)((char *)s_terrain_d__4125_2316 + 2 + iVar1 * 0x10),
                  *(undefined2 *)((char *)s_terrain_d__4125_2316 + 4 + iVar1 * 0x10),param_2);
  }
  if (param_3 != 0) {
    FUN_7ecb_00d6(*(undefined2 *)((char *)s_terrain_d__4125_2316 + 6 + iVar1 * 0x10),
                  *(undefined2 *)((char *)s_terrain_d__4125_2316 + 8 + iVar1 * 0x10),param_3);
  }
  FUN_7ecb_0058(DAT_4125_236c,DAT_4125_2370,0x58d8,0x4125);
  FUN_22bf_0757();
  return;
}


