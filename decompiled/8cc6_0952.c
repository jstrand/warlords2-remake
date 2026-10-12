
void __cdecl16far draw_turn_strip(int param_1,int param_2)

{
  undefined1 *puVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_SS;
  int local_68 [7];
  uint local_5a [9];
  undefined2 local_48 [8];
  int local_38 [8];
  char local_28 [20];
  int local_14;
  int local_12;
  int local_10;
  undefined1 local_e [4];
  undefined1 local_a [8];
  
  iVar5 = 0;
  for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
    if (*(int *)(iVar3 * 2 + 0x137) != 0) {
      iVar5 = iVar5 + 1;
    }
  }
  if (iVar5 == 0) {
    return;
  }
  for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
    local_38[iVar3] = 0;
    local_48[iVar3] = 0xffff;
    local_5a[iVar3 + 1] = 0;
    local_68[iVar3] = 0;
  }
  iVar3 = 0;
  local_14 = iVar5;
  for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
    if (*(int *)(*(int *)(iVar4 * 2 + 0x163) * 2 + 0x137) != 0) {
      local_48[iVar3] = *(undefined2 *)(iVar4 * 2 + 0x163);
      local_38[iVar3] = ((8 - iVar5) + iVar3) * 0x10 + 0x200;
      if (*(int *)(iVar4 * 2 + 0x163) == current_player) {
        local_12 = iVar3;
      }
      iVar3 = iVar3 + 1;
    }
  }
  iVar3 = local_14;
  if ((local_12 == 0) || (iVar5 == 1)) {
    local_5a[1] = (uint)(iVar5 != 1);
  }
  else {
    if (local_14 + -1 == local_12) {
      local_5a[local_14] = (uint)(1 < iVar5);
      puVar1 = &stack0xff94;
      goto LAB_8cc6_0aae;
    }
    local_5a[local_12 + 1] = (uint)(1 < iVar5);
    iVar3 = local_12;
  }
  puVar1 = &stack0xff96;
LAB_8cc6_0aae:
  *(undefined2 *)(puVar1 + iVar3 * 2) = 1;
  FUN_22bf_07a5();
  if (param_1 != 0) {
    FUN_24d0_0002(0xf);
    FUN_216d_0288(0x3256,0x4125);
    FUN_78a8_06ae(2,0,0xf,3);
    uVar2 = game_turn();
    sprintf((char *)CONCAT22(unaff_SS,local_28),s_Turn__d_4125_326b,uVar2);
    FUN_7ecb_0103((8 - iVar5) * 0x10 + 0x1fc,0,local_28);
    for (iVar3 = 0; iVar3 < local_14; iVar3 = iVar3 + 1) {
      FUN_216d_000e(local_e);
      FUN_216d_002e(local_a);
      __F_SPUSH(local_e._0_2_,local_e._2_2_);
      FUN_1997_027e(9);
    }
  }
  iVar3 = -1;
  local_10 = -1;
  for (iVar5 = 0; iVar5 < local_14; iVar5 = iVar5 + 1) {
    if (local_68[iVar5] != 0) {
      iVar3 = iVar5;
    }
  }
  for (iVar5 = 0; iVar5 < local_14; iVar5 = iVar5 + 1) {
    if (local_5a[iVar5 + 1] != 0) {
      local_10 = iVar5;
    }
  }
  if (((param_1 == 0) && (-1 < iVar3)) && (param_2 == 0)) {
    FUN_24d0_0002(0xf);
    FUN_216d_002e(local_a);
    FUN_216d_0288(local_a);
    FUN_216d_000e(local_e);
    FUN_216d_002e(local_a);
    __F_SPUSH(local_e._0_2_,local_e._2_2_);
    FUN_1997_027e(9);
  }
  if (((param_1 == 0) && (-1 < local_10)) && (param_2 == 0)) {
    FUN_24d0_0002(0);
    FUN_216d_002e(local_a);
    FUN_216d_0288(local_a);
    FUN_216d_000e(local_e);
    FUN_216d_002e(local_a);
    __F_SPUSH(local_e._0_2_,local_e._2_2_);
    FUN_1997_027e(9);
  }
  FUN_22bf_0757();
  return;
}


