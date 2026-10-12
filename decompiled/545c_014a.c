
void __cdecl16far auto_str_menu_shortcuts(int param_1)

{
  int iVar1;
  undefined2 unaff_SS;
  undefined1 *local_4e;
  int local_1a [2];
  int local_16;
  undefined1 local_a [4];
  int local_6;
  
  FUN_22bf_07a5();
  if (param_1 == 0) {
    FUN_78a8_06ae(0xf0001,0x30000);
    FUN_7ecb_00d6(DAT_4125_0552,0x55c,0x4125);
  }
  FUN_78a8_06ae(0xf0002,0x30000);
  for (iVar1 = 0; iVar1 < 0x15; iVar1 = iVar1 + 1) {
    auto_file_udb_dat();
    local_16 = local_16 / 2;
    if (((undefined2 *)&DAT_451b_004a)[DAT_4125_055a] == iVar1) {
      local_1a[0] = local_1a[0] + local_16;
    }
    __F_SPUSH(*(undefined4 *)(iVar1 * 4 + 0x4ce));
    FUN_1997_0129();
    if (param_1 == 0) {
      local_4e = &stack0xffb4;
      FUN_7ecb_00bf(*(int *)(iVar1 * 4 + 0x4ce) + 0x28,*(int *)(iVar1 * 4 + 0x4d0) + 5);
    }
  }
  if (param_1 == 0) {
    FUN_78a8_06ae(0xf0002,0x30000);
    FUN_7ecb_00d6(DAT_4125_0556,0x56b,0x4125);
  }
  for (iVar1 = 0; iVar1 < 4; iVar1 = iVar1 + 1) {
    if ((int)((undefined2 *)&DAT_451b_004a)[iVar1] < 0) {
      __F_SCOPY(0x49e,0x4125,local_a,unaff_SS);
      ((undefined2 *)&DAT_451b_0042)[iVar1] = 0;
    }
    else {
      auto_file_udb_dat();
      __F_SCOPY(local_1a,unaff_SS,local_a,unaff_SS);
      ((undefined2 *)&DAT_451b_0042)[iVar1] = local_4e;
    }
    local_6 = local_6 / 2;
    __F_SPUSH();
    FUN_1997_0129();
    if (iVar1 == DAT_4125_055a) {
      FUN_24d0_0002();
      local_4e = (undefined1 *)(iVar1 * 8 + 0x532);
      FUN_216d_01fd();
    }
  }
  FUN_22bf_0757();
  return;
}


