
undefined2 __cdecl16far auto_str_scan_dist_status_bonus(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  int local_20 [8];
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  undefined2 local_8;
  int local_6;
  uint local_4;
  
  local_8 = 0;
  uVar6 = (undefined2)((ulong)param_3 >> 0x10);
  iVar4 = (int)param_3;
  iVar1 = *(int *)(iVar4 + param_1 * 2);
  if (param_2 == 3) {
    iVar2 = *(int *)(*(int *)(iVar4 + 0x34) * 0x1f + 0x811);
    local_c = *(int *)(*(int *)(iVar4 + 0x34) * 0x1f + 0x813);
    iVar5 = *(int *)(iVar4 + 0x34);
  }
  else {
    iVar2 = *(int *)(*(int *)(iVar4 + 0x3c) * 0x1d + 0xd02);
    local_c = *(int *)(*(int *)(iVar4 + 0x3c) * 0x1d + 0xd04);
    iVar5 = *(int *)(iVar4 + 0x3c) + 1000;
  }
  local_e = map_distance(iVar2,local_c,*(int *)(iVar1 * 0x16 + -0x7780),
                         *(int *)(iVar1 * 0x16 + -0x777e));
  local_4 = ai_collect_stack(*(int *)(iVar1 * 0x16 + -0x7780),*(int *)(iVar1 * 0x16 + -0x777e),
                             (int *)CONCAT22(unaff_SS,local_20),
                             (*(byte *)(iVar1 * 0x16 + -0x7772) & 0x70) >> 4,
                             *(byte *)(iVar1 * 0x16 + -0x7772) & 0xf,0xc);
  if (local_4 == 0) {
    return local_8;
  }
  local_a = *(int *)(iVar4 + param_1 * 2 + 0xc);
  if (local_a == -1) {
    local_6 = 100;
  }
  else {
    local_6 = FUN_57ea_0a3e(*(undefined2 *)(iVar1 * 0x16 + -0x7780),
                            *(undefined2 *)(iVar1 * 0x16 + -0x777e),0xffff);
  }
  if (local_a != -1) {
    iVar4 = -1;
    iVar2 = 8;
    while (iVar3 = iVar2 + -1, iVar2 != 0) {
      local_10 = local_20[iVar3];
      iVar2 = iVar3;
      if ((local_10 != -1) &&
         (((undefined1 *)&army_move_flags)[*(char *)(local_10 * 0x16 + -0x777c) * 6] != '\0')) {
        iVar4 = local_10;
      }
    }
    if (iVar4 == -1) {
      if (local_6 < 0xf) goto LAB_6087_0a0c;
      iVar4 = 8;
      while (iVar4 != 0) {
        local_20[iVar4 + -1] = -1;
        iVar4 = iVar4 + -1;
      }
      local_4 = 1;
    }
    else {
      iVar2 = 8;
      while (iVar2 != 0) {
        local_20[iVar2 + -1] = -1;
        iVar2 = iVar2 + -1;
      }
      local_4 = 2;
      local_20[1] = iVar4;
    }
    local_20[0] = iVar1;
    FUN_623c_14d3(local_20);
  }
LAB_6087_0a0c:
  if ((((local_a == -1) || (*(byte *)((int)DAT_451b_018e + local_a + 0x182) != local_4)) ||
      (2 < (int)local_4)) || (0xe < local_6)) {
    if ((local_a != -1) && (local_4 == 3)) {
      iVar4 = 8;
      while (iVar4 != 0) {
        local_20[iVar4 + -1] = -1;
        iVar4 = iVar4 + -1;
      }
      local_20[0] = iVar1;
      FUN_623c_14d3(local_20);
    }
    stack_movement_mode();
    sprintf(&DAT_451b_1d28,s_Scan_dist__d__status__d_bonus__d_4125_0960,local_e + 10,DAT_451b_1e9c,
            DAT_451b_1e9a);
    FUN_623c_170c();
    if (iVar5 != -1) {
      sprintf(&DAT_451b_1d28,s_Moving_to_site__d_4125_0981,iVar5);
      if (iVar5 < 1000) {
        FUN_623c_0ae7(local_20);
        iVar1 = move_stack_to();
      }
      else {
        FUN_623c_0ae7(local_20);
        *(undefined2 *)((int)DAT_451b_1f02 + 0x12) = *(undefined2 *)(iVar5 * 0x1d + -0x6446);
        *(undefined2 *)((int)DAT_451b_1f02 + 0x14) = *(undefined2 *)(iVar5 * 0x1d + -0x6444);
        iVar1 = move_stack_to();
      }
      if (iVar1 == 4) {
        local_8 = 1;
      }
    }
  }
  return local_8;
}


