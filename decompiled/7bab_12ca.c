
void __cdecl16far auto_ui_difficulty_affecting(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  int index;
  char *pcVar3;
  undefined1 local_1a [8];
  int local_12;
  int local_10;
  uint local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_e = (uint)(param_1 < 6);
  FUN_22bf_07a5();
  local_4 = param_1;
  index = param_1;
  if (param_1 == -1) {
    local_4 = 9;
    index = 0;
    if (DAT_4125_166a == 0) {
      FUN_78a8_06ae(0xf0001,0x30000);
      pcVar3 = get_string(8,0);
      FUN_7ecb_00d6(0x370140,(char *)pcVar3,(int)((ulong)pcVar3 >> 0x10));
      FUN_78a8_06ae(0xf0002,0x30000);
      FUN_24d0_0002();
      FUN_2133_02fe();
      FUN_2133_02fe();
      FUN_24d0_0002();
      FUN_2133_02fe();
      FUN_2133_02fe();
      pcVar3 = get_string(9,0);
      local_10 = FUN_21e2_0a8e((char *)pcVar3,(int)((ulong)pcVar3 >> 0x10));
      pcVar3 = get_string(10,0);
      local_12 = FUN_21e2_0a8e((char *)pcVar3,(int)((ulong)pcVar3 >> 0x10));
      FUN_24d0_0002();
      iVar2 = 0x13c - local_10 / 2;
      uVar1 = FUN_216d_002e(local_1a);
      FUN_216d_0288(uVar1,iVar2);
      iVar2 = 0x13c - local_12 / 2;
      uVar1 = FUN_216d_002e(local_1a);
      FUN_216d_0288(uVar1,iVar2);
      pcVar3 = get_string(9,0);
      FUN_7ecb_00d6(0x680140,(char *)pcVar3,(int)((ulong)pcVar3 >> 0x10));
      get_string(10,0);
      FUN_7ecb_00d6(0xf30140);
    }
  }
  do {
    if (local_4 < index) {
      if ((param_1 == -1) || (local_e != 0)) {
        difficulty_rating();
      }
      FUN_22bf_0757();
      return;
    }
    if (index < 6) {
      local_8 = (index / 2) * 0x1e + 0x83;
      if (index / 2 == (index + 1) / 2) goto LAB_7bab_14a5;
LAB_7bab_14dc:
      local_6 = 0x140;
    }
    else {
      local_8 = ((index + -6) / 2) * 0x1e + 0x10e;
      if ((index + -6) / 2 != (index + -5) / 2) goto LAB_7bab_14dc;
LAB_7bab_14a5:
      local_6 = 0x80;
    }
    local_a = local_6 + 0x80;
    local_c = local_8;
    if (param_1 == -1) {
      FUN_78a8_06ae(0xf0002,0x30000);
      pcVar3 = get_string(4,index);
      FUN_7ecb_00bf(local_6,local_8,(char *)pcVar3,(int)((ulong)pcVar3 >> 0x10));
    }
    FUN_54f6_070b(local_a + -0x78,local_c + -0x32,0x140038);
    FUN_78a8_06ae(0x70002,0x30000);
    if (index == 0) {
      get_string(5,DAT_4125_23b4);
    }
    FUN_7ecb_00bf();
    index = index + 1;
  } while( true );
}


