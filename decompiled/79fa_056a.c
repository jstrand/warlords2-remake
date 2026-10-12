
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_str_join_data_not_found(int param_1)

{
  bool bVar1;
  int *piVar2;
  undefined2 uVar3;
  uint in_DX;
  uint extraout_DX;
  uint extraout_DX_00;
  uint uVar4;
  int iVar5;
  undefined2 unaff_SS;
  undefined1 local_4c [2];
  int local_4a;
  byte local_48;
  undefined1 local_2a [2];
  int local_28;
  uint local_26;
  uint local_24;
  undefined4 local_1c;
  undefined4 local_18;
  int *local_14;
  undefined4 local_10;
  uint local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  piVar2 = (int *)mem_lock(DAT_451b_0cec);
  local_10 = (int *)CONCAT22(in_DX,piVar2);
  local_14 = local_10;
  bVar1 = false;
  for (local_4 = 0; local_4 < 0x24; local_4 = local_4 + 1) {
    local_6 = *local_14;
    piVar2 = (int *)local_14;
    local_14 = (int *)CONCAT22(local_14._2_2_,(int *)local_14 + 1);
    if (local_6 == param_1) {
      local_a = *local_14;
      local_14 = (int *)CONCAT22(local_14._2_2_,piVar2 + 2);
      local_8 = *local_14;
      bVar1 = true;
    }
    local_14 = (int *)CONCAT22(local_14._2_2_,piVar2 + 3);
  }
  mem_unlock(DAT_451b_0cec);
  if (!bVar1) {
    auto_str_fatal_error((char *)s_Join_data_not_found_4125_175a,0x4125);
  }
  uVar3 = mem_lock(DAT_451b_0ce8);
  local_10 = (int *)CONCAT22(in_DX,uVar3);
  local_18 = local_10;
  bVar1 = false;
  local_4 = 0;
  while( true ) {
    if (6 < local_4) goto LAB_79fa_068c;
    __F_SCOPY(local_18,local_2a,unaff_SS);
    local_18 = CONCAT22(local_18._2_2_,(int)local_18 + 0xe);
    if (local_28 == local_8) break;
    for (iVar5 = 0; iVar5 < (int)local_24; iVar5 = iVar5 + 1) {
      local_18 = CONCAT22(local_18._2_2_,(int)local_18 + 0xe);
    }
    local_4 = local_4 + 1;
    in_DX = local_24;
  }
  local_c = local_26;
  in_DX = extraout_DX;
  for (uVar4 = 0; uVar4 < local_24; uVar4 = uVar4 + 1) {
    __F_SCOPY(local_18,DAT_4125_168e + uVar4 * 0xe,(int)((ulong)_DAT_4125_168e >> 0x10));
    local_18 = CONCAT22(local_18._2_2_,(int)local_18 + 0xe);
    in_DX = extraout_DX_00;
  }
  bVar1 = true;
  DAT_4125_1686 = local_24;
LAB_79fa_068c:
  mem_unlock(DAT_451b_0ce8);
  if (!bVar1) {
    auto_str_fatal_error((char *)s_Area_data_not_found_4125_176e,0x4125);
  }
  uVar3 = mem_lock(DAT_451b_0ce4);
  local_10 = (int *)CONCAT22(in_DX,uVar3);
  local_1c = local_10;
  bVar1 = false;
  local_4 = 0;
  while( true ) {
    if (0x23 < local_4) goto LAB_79fa_0744;
    __F_SCOPY(local_1c,local_4c,unaff_SS);
    local_1c = CONCAT22(local_1c._2_2_,(int)local_1c + 0x21);
    if (local_4a == local_a) break;
    for (iVar5 = 0; iVar5 < (int)(uint)local_48; iVar5 = iVar5 + 1) {
      local_1c = CONCAT22(local_1c._2_2_,(int)local_1c + 0x21);
    }
    local_4 = local_4 + 1;
  }
  for (iVar5 = 0; iVar5 < (int)(uint)local_48; iVar5 = iVar5 + 1) {
    __F_SCOPY(local_1c,DAT_4125_168a + iVar5 * 0x21,(int)((ulong)_DAT_4125_168a >> 0x10));
    local_1c = CONCAT22(local_1c._2_2_,(int)local_1c + 0x21);
  }
  DAT_4125_1688 = (uint)local_48;
  bVar1 = true;
LAB_79fa_0744:
  mem_unlock(DAT_451b_0ce4);
  if (!bVar1) {
    auto_str_fatal_error((char *)s_Button_data_not_found_4125_1782,0x4125);
  }
  DAT_4125_1692 = _DAT_4125_168a;
  DAT_4125_1696 = _DAT_4125_168e;
  DAT_4125_169c = 0x4125;
  DAT_4125_169a = (undefined2 *)&DAT_4125_1688;
  DAT_4125_16a0 = 0x4125;
  DAT_4125_169e = (undefined2 *)&DAT_4125_1686;
  DAT_451b_0ce0._2_2_ = 0x4125;
  DAT_451b_0ce0._0_2_ = (undefined4 *)&DAT_4125_1692;
  FUN_18a9_0345();
  for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1) {
    if ((local_c & 1 << ((byte)local_4 & 0x1f)) != 0) {
      FUN_18a9_07ed();
    }
  }
  return;
}


