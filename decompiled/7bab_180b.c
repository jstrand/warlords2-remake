
void __cdecl16far auto_str_setup_side(int param_1)

{
  undefined2 uVar1;
  undefined2 in_DX;
  int local_a;
  int local_8;
  
  FUN_22bf_07a5();
  if (param_1 == 0) {
    FUN_78a8_06ae(0xf0001,0x30000);
    FUN_7ecb_00d6(0x370140,(char *)s_Setup_Side_4125_25b3,0x4125);
    FUN_216d_002e(&local_a);
    FUN_24d0_0002();
    FUN_216d_01fd(&local_a);
    local_a = local_a + -1;
    local_8 = local_8 + -1;
    FUN_24d0_0002();
    FUN_216d_01fd(&local_a);
    local_a = local_a + -1;
    local_8 = local_8 + -1;
    FUN_216d_01fd(&local_a);
    FUN_216d_002e(&local_a);
    FUN_24d0_0002();
    FUN_216d_01fd(&local_a);
    local_a = local_a + -1;
    local_8 = local_8 + -1;
    FUN_24d0_0002();
    FUN_216d_01fd(&local_a);
    local_a = local_a + -1;
    local_8 = local_8 + -1;
    FUN_216d_01fd(&local_a);
    FUN_78a8_06ae(2,*(undefined2 *)(DAT_4125_2563 * 2 + 0xa0),
                  *(undefined2 *)(DAT_4125_2563 * 2 + 0xb0));
    FUN_21e2_0a8e((char *)s_Side_Name_4125_25be);
    FUN_24d0_0002();
    uVar1 = FUN_216d_002e(&local_a);
    FUN_216d_0288(uVar1,in_DX);
    FUN_7ecb_00bf(0x6000a0,(char *)s_Side_Name_4125_25c8,0x4125);
    FUN_21e2_0a8e((char *)s_Leader_4125_25d2,0x4125);
    FUN_24d0_0002();
    uVar1 = FUN_216d_002e(&local_a);
    FUN_216d_0288(uVar1,in_DX);
    FUN_7ecb_00bf(0xb500a0,(char *)s_Leader_4125_25d9,0x4125);
    FUN_8611_0bf7(DAT_4125_2563,0,0x342a,0x4125,0x7a0090);
    FUN_8611_0bf7(DAT_4125_2563,0,0x342a,0x4125,0x7a01c8);
    FUN_8611_0bf7(DAT_4125_2563,0,0x342a,0x4125,0xca0090);
    FUN_78a8_06ae(0xf0002,0x30000);
    FUN_7ecb_00d6(0x740140,(char *)s_Retype_the_name_of_this_side_4125_25e0,0x4125);
    FUN_7ecb_0058(0x8a00e5,0x1600bc,DAT_4125_2563 * 0x14,0x3c04);
    FUN_24d0_0002();
    FUN_2133_02fe();
    FUN_2133_02fe();
    FUN_7ecb_00bf(0xcb00c4,0x25fd,0x4125);
    FUN_7ecb_00bf(0xcb0180,(char *)s_Description_4125_2602,0x4125);
    FUN_7bab_1be6();
    FUN_7bab_1d0f();
  }
  FUN_22bf_0757();
  return;
}


