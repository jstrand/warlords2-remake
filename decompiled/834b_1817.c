
void __cdecl16far FUN_834b_1817(int *param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  int local_fa [100];
  int local_32;
  uint local_30;
  int local_2e;
  int local_2c [4];
  undefined4 local_24;
  undefined4 local_20;
  undefined1 local_1c [8];
  int local_14;
  int local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_32 = 0;
  if (param_1 == (int *)0x0) {
    __F_SCOPY(0x2a88,0x4125,&local_24,unaff_SS);
    FUN_216d_002e(local_1c);
  }
  else {
    __F_SCOPY(param_1,local_1c,unaff_SS);
    uVar6 = (undefined2)((ulong)param_1 >> 0x10);
    piVar1 = (int *)param_1;
    local_24 = CONCAT22(piVar1[1] << 1,*param_1 << 1);
    local_20 = CONCAT22(piVar1[3] << 1,piVar1[2] << 1);
  }
  FUN_1997_034a();
  FUN_1997_060a();
  if (opt_hidden_map == 0) {
    (*DAT_451b_2a74)();
  }
  else {
    FUN_24d0_0497();
    (*DAT_451b_2a68)(0x24d0,0x581d,0x4125,local_24,local_20,0x6795,0x4125,local_24,0x54e3,0x4125,
                     local_24);
  }
  FUN_1997_06bb();
  for (iVar2 = 0; iVar2 < city_count; iVar2 = iVar2 + 1) {
    local_fa[iVar2] = 0;
  }
  for (iVar2 = 0; iVar2 < city_count; iVar2 = iVar2 + 1) {
    if ((*(char *)(iVar2 * 0x41 + 0x1592) == current_player) && (local_fa[iVar2] == 0)) {
      local_fa[iVar2] = 1;
      FUN_7087_0410();
      if (*(char *)(iVar2 * 0x41 + 0x15aa) < '\x01') {
        local_6 = 3;
      }
      else {
        local_6 = 0;
      }
      if (*(int *)((undefined1 *)&LAB_4125_15ad_1 + iVar2 * 0x41) != 0) {
        local_6 = 2;
      }
      if (0 < local_2e) {
        if (*(char *)(iVar2 * 0x41 + 0x15aa) < '\x01') {
          local_6 = 5;
        }
        else {
          local_6 = 1;
        }
      }
      local_8 = *(int *)(iVar2 * 0x41 + 0x157d) * 2;
      uVar3 = local_8 - 2;
      local_a = *(int *)(iVar2 * 0x41 + 0x157f) * 2;
      local_4 = local_a + -1;
      local_8 = local_8 + 2;
      local_a = local_a + 2;
      local_30 = uVar3 & 7;
      if (local_30 == 7) {
        local_30 = 6;
      }
      FUN_1997_034a();
      (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,*(undefined2 *)(local_6 * 8 + 0x2c76),
                       *(int *)(local_6 * 8 + 0x2c78) + local_30 * *(int *)(local_6 * 8 + 0x2c7c),
                       *(undefined2 *)(local_6 * 8 + 0x2c7a),*(undefined2 *)(local_6 * 8 + 0x2c7c),
                       0x6795,0x4125,((int)uVar3 >> 3) << 3,local_4,0x576c,0x4125,
                       *(undefined2 *)(local_6 * 8 + 0x2c76),
                       *(int *)(local_6 * 8 + 0x2c78) + local_30 * *(int *)(local_6 * 8 + 0x2c7c));
      if ((*(int *)((undefined1 *)&LAB_4125_15ad_1 + iVar2 * 0x41) != 0) &&
         (*(int *)((undefined1 *)&LAB_4125_15b0 + iVar2 * 0x41) == -2)) {
        local_32 = 2;
      }
      if ((*(int *)((undefined1 *)&LAB_4125_15ad_1 + iVar2 * 0x41) != 0) &&
         (*(int *)((undefined1 *)&LAB_4125_15b0 + iVar2 * 0x41) != -2)) {
        iVar5 = FUN_828e_044a(*(undefined2 *)((undefined1 *)&LAB_4125_15b0 + iVar2 * 0x41),
                              *(undefined2 *)(iVar2 * 0x41 + 0x15b2));
        local_fa[iVar5] = 1;
        iVar4 = *(int *)(iVar5 * 0x41 + 0x157d) * 2;
        local_e = *(int *)(iVar5 * 0x41 + 0x157f) * 2;
        local_4 = local_e + -1;
        local_c = iVar4 + 2;
        local_e = local_e + 2;
        if (*(char *)(iVar5 * 0x41 + 0x15aa) < '\x01') {
          local_6 = 5;
        }
        else {
          local_6 = 1;
        }
        local_30 = iVar4 - 2U & 7;
        if (local_30 == 7) {
          local_30 = 6;
        }
        FUN_1997_034a();
        (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,*(undefined2 *)(local_6 * 8 + 0x2c76),
                         *(int *)(local_6 * 8 + 0x2c78) + local_30 * *(int *)(local_6 * 8 + 0x2c7c),
                         *(undefined2 *)(local_6 * 8 + 0x2c7a));
        FUN_24d0_0002();
        FUN_2133_00b8(0x6795,0x4125,local_c,local_e,local_8,local_a,0);
      }
      for (local_10 = 0; local_10 < local_2e; local_10 = local_10 + 1) {
        local_fa[local_2c[local_10]] = 1;
        iVar5 = *(int *)(local_2c[local_10] * 0x41 + 0x157d) * 2;
        local_e = *(int *)(local_2c[local_10] * 0x41 + 0x157f) * 2;
        local_4 = local_e + -1;
        local_c = iVar5 + 2;
        local_e = local_e + 2;
        local_6 = 2;
        local_30 = iVar5 - 2U & 7;
        if (local_30 == 7) {
          local_30 = 6;
        }
        FUN_1997_034a();
        (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,*(undefined2 *)(local_6 * 8 + 0x2c76),
                         *(int *)(local_6 * 8 + 0x2c78) + local_30 * *(int *)(local_6 * 8 + 0x2c7c),
                         *(undefined2 *)(local_6 * 8 + 0x2c7a));
        FUN_24d0_0002();
        FUN_2133_00b8(0x6795,0x4125,local_c,local_e,local_8,local_a,0);
      }
    }
  }
  iVar2 = FUN_7563_0b47();
  if ((iVar2 != 0) && (local_32 == 0)) {
    local_32 = 1;
  }
  if ((iVar2 != 0) && (FUN_834b_1f5f(), local_32 == 2)) {
    for (iVar2 = 0; iVar2 < 100; iVar2 = iVar2 + 1) {
      if (((*(int *)((undefined1 *)&LAB_4125_15ad_1 + iVar2 * 0x41) != 0) &&
          (*(int *)((undefined1 *)&LAB_4125_15b0 + iVar2 * 0x41) == -2)) &&
         (*(char *)(iVar2 * 0x41 + 0x1592) == current_player)) {
        local_a = *(int *)(iVar2 * 0x41 + 0x157f) * 2;
        local_4 = local_a + -1;
        local_8 = *(int *)(iVar2 * 0x41 + 0x157d) * 2 + 2;
        local_a = local_a + 2;
        local_c = local_12 << 1;
        local_e = local_14 << 1;
        FUN_24d0_0002();
        FUN_2133_00b8(0x6795,0x4125,local_8,local_a,local_c,local_e,0);
      }
    }
  }
  if (-1 < param_2) {
    iVar5 = *(int *)(param_2 * 0x41 + 0x157d) * 2;
    iVar2 = *(int *)(param_2 * 0x41 + 0x157f) * 2;
    local_4 = iVar2 + -1;
    FUN_2012_0bb7(0x6795,0x4125,iVar5 + -3,iVar2 + -2,0xf000b);
    FUN_2012_0bb7(0x6795,0x4125,iVar5 + -3,local_4 + 10,0xf000b);
    FUN_2012_0f08(0x6795,0x4125,iVar5 + -3,local_4 + -1,0xf000b);
    FUN_2012_0f08(0x6795,0x4125,iVar5 + 8,local_4 + -1,0xf000b);
  }
  return;
}


