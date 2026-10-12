
void __cdecl16far FUN_834b_08df(int *param_1,int param_2,int param_3)

{
  bool bVar1;
  int iVar2;
  int *piVar3;
  uint uVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
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
  
  bVar1 = false;
  if (param_1 == (int *)0x0) {
    __F_SCOPY(0x2a88,0x4125,&local_24,unaff_SS);
    FUN_216d_002e(local_1c);
  }
  else {
    __F_SCOPY(param_1,local_1c,unaff_SS);
    uVar5 = (undefined2)((ulong)param_1 >> 0x10);
    piVar3 = (int *)param_1;
    local_24 = CONCAT22(piVar3[1] << 1,*param_1 << 1);
    local_20 = CONCAT22(piVar3[3] << 1,piVar3[2] << 1);
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
  local_4 = 0;
  do {
    if (city_count <= local_4) {
      if (-1 < param_2) {
        if ((*(int *)((undefined1 *)&LAB_4125_15ad_1 + param_2 * 0x41) != 0) &&
           (*(int *)((undefined1 *)&LAB_4125_15b0 + param_2 * 0x41) == -2)) {
          bVar1 = true;
          iVar2 = FUN_7563_0b47();
          if (iVar2 != 0) {
            local_a = local_12 << 1;
            local_e = local_14 << 1;
            FUN_834b_1f5f();
            FUN_24d0_0002();
            FUN_2133_00b8(0x6795,0x4125,local_8,local_c,local_a,local_e,0);
          }
        }
        if ((*(int *)((undefined1 *)&LAB_4125_15ad_1 + param_2 * 0x41) != 0) &&
           (*(int *)((undefined1 *)&LAB_4125_15b0 + param_2 * 0x41) != -2)) {
          iVar2 = FUN_828e_044a(*(undefined2 *)((undefined1 *)&LAB_4125_15b0 + param_2 * 0x41),
                                *(undefined2 *)(param_2 * 0x41 + 0x15b2));
          local_a = *(int *)(iVar2 * 0x41 + 0x157d) * 2;
          uVar4 = local_a - 2;
          local_e = *(int *)(iVar2 * 0x41 + 0x157f) * 2;
          local_6 = local_e + -1;
          local_a = local_a + 2;
          local_e = local_e + 2;
          if (*(char *)(iVar2 * 0x41 + 0x15aa) < '\x01') {
            iVar2 = 5;
          }
          else {
            iVar2 = 1;
          }
          local_30 = uVar4 & 7;
          if (local_30 == 7) {
            local_30 = 6;
          }
          FUN_1997_034a();
          (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,*(undefined2 *)(iVar2 * 8 + 0x2c76),
                           *(int *)(iVar2 * 8 + 0x2c78) + local_30 * *(int *)(iVar2 * 8 + 0x2c7c),
                           *(undefined2 *)(iVar2 * 8 + 0x2c7a),*(undefined2 *)(iVar2 * 8 + 0x2c7c),
                           0x6795,0x4125,((int)uVar4 >> 3) << 3,local_6,0x576c,0x4125,
                           *(undefined2 *)(iVar2 * 8 + 0x2c76),
                           *(int *)(iVar2 * 8 + 0x2c78) + local_30 * *(int *)(iVar2 * 8 + 0x2c7c));
          FUN_24d0_0002();
          FUN_2133_00b8(0x6795,0x4125,local_8,local_c,local_a,local_e,0);
        }
        FUN_7087_0410();
        for (local_4 = 0; local_4 < local_2e; local_4 = local_4 + 1) {
          local_a = *(int *)(local_2c[local_4] * 0x41 + 0x157d) * 2;
          uVar4 = local_a - 2;
          local_e = *(int *)(local_2c[local_4] * 0x41 + 0x157f) * 2;
          local_6 = local_e + -1;
          local_a = local_a + 2;
          local_e = local_e + 2;
          local_30 = uVar4 & 7;
          if (local_30 == 7) {
            local_30 = 6;
          }
          FUN_1997_034a();
          (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,DAT_4125_2c86,
                           DAT_4125_2c88 + local_30 * DAT_4125_2c8c,DAT_4125_2c8a,DAT_4125_2c8c,
                           0x6795,0x4125,((int)uVar4 >> 3) << 3,local_6,0x576c,0x4125,DAT_4125_2c86,
                           DAT_4125_2c88 + local_30 * DAT_4125_2c8c);
          FUN_24d0_0002();
          FUN_2133_00b8();
        }
      }
      local_10 = FUN_7563_0b47();
      if ((local_10 != 0) && (!bVar1)) {
        FUN_834b_1f5f();
      }
      return;
    }
    if (*(char *)(local_4 * 0x41 + 0x1592) == current_player) {
      if (param_3 == -1) {
        FUN_7087_0410();
        iVar2 = local_2e + 1;
joined_r0x00083ef6:
        if ((4 < iVar2) && (local_4 != param_2)) goto LAB_834b_0b5d;
      }
      else if (-1 < param_3) {
        FUN_7087_0410();
        iVar2 = local_2e + param_3;
        goto joined_r0x00083ef6;
      }
      iVar2 = *(int *)(local_4 * 0x41 + 0x157f) * 2;
      local_6 = iVar2 + -1;
      if ((-1 < param_2) && (param_2 == local_4)) {
        local_8 = *(int *)(local_4 * 0x41 + 0x157d) * 2 + 2;
        local_c = iVar2 + 2;
      }
      FUN_1997_034a();
      (*DAT_451b_2a68)(0x1997,0x57a7,0x4125);
    }
LAB_834b_0b5d:
    local_4 = local_4 + 1;
  } while( true );
}


