
void __cdecl16far FUN_623c_010b(void)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 unaff_SS;
  int local_38 [8];
  undefined2 local_28 [8];
  undefined2 local_18 [8];
  int local_8;
  int local_6;
  int local_4;
  
  iVar4 = 0;
  local_8 = 0;
  FUN_22bf_07a5();
  auto_ui_setting_up_game();
  auto_str_no_room_ai_data();
  FUN_623c_0398();
  FUN_5db9_063d();
  FUN_8065_1452();
  FUN_22bf_0757();
  local_4 = 8;
  while( true ) {
    iVar1 = local_4;
    local_4 = local_4 + -1;
    if (iVar1 == 0) break;
    local_38[local_4] = 0;
    if (*(int *)(local_4 * 2 + 0x137) != 0) {
      if (*(int *)(local_4 * 2 + 0xd0) == 0) {
        iVar4 = iVar4 + 1;
      }
      else {
        local_38[local_4] = 1;
        local_8 = local_8 + 1;
      }
      local_18[local_4] = *(undefined2 *)(local_4 * 0x14 + 0x189);
      local_28[local_4] = *(undefined2 *)(local_4 * 0x14 + 0x18b);
      iVar1 = FUN_828e_044a();
      if (-1 < iVar1) {
        *(undefined1 *)(iVar1 * 0x41 + 0x15ac) = (undefined1)local_4;
      }
    }
  }
  if ((local_8 == 0) && (iVar4 != 0)) {
    local_4 = 8;
    iVar4 = -1;
    iVar1 = 0;
    while (iVar2 = local_4 + -1, local_4 != 0) {
      local_4 = iVar2;
      if (*(int *)(iVar2 * 2 + 0x137) != 0) {
        local_38[iVar2] = 1;
        iVar2 = dice(1,100,0);
        if ((iVar4 == -1) || (iVar1 < iVar2)) {
          iVar4 = local_4;
          iVar1 = iVar2;
        }
      }
    }
    if (iVar4 != -1) {
      local_38[iVar4] = 0;
    }
  }
  local_6 = -1;
  local_4 = 8;
  do {
    iVar4 = local_4;
    local_4 = local_4 + -1;
    if (iVar4 == 0) goto LAB_623c_02b0;
  } while (local_38[local_4] == 0);
  local_6 = local_4;
LAB_623c_02b0:
  if (local_6 != -1) {
    while (iVar4 = FUN_623c_0a27(local_18[local_6],local_28[local_6],0xf), -1 < iVar4) {
      *(undefined1 *)(iVar4 * 0x41 + 0x15ac) = (undefined1)local_6;
      iVar1 = dice(1,10,-1);
      if (iVar1 < 5) {
        local_18[local_6] = *(undefined2 *)(local_6 * 0x14 + 0x189);
        uVar3 = *(undefined2 *)(local_6 * 0x14 + 0x18b);
      }
      else {
        local_18[local_6] = *(undefined2 *)(iVar4 * 0x41 + 0x157d);
        uVar3 = *(undefined2 *)(iVar4 * 0x41 + 0x157f);
      }
      local_28[local_6] = uVar3;
      do {
        local_6 = (local_6 + 1) % 8;
      } while (local_38[local_6] == 0);
    }
  }
  return;
}


