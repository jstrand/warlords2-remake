
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far after_battle(int param_1,int param_2,int param_3)

{
  byte *pbVar1;
  int *piVar2;
  byte bVar3;
  undefined1 uVar4;
  int iVar5;
  undefined2 uVar6;
  uint uVar7;
  int *piVar8;
  int iVar9;
  char *fmt;
  
  FUN_67cc_1aff();
  iVar5 = FUN_828e_044a(param_2,param_3);
  if ((((-1 < iVar5) && (7 < (int)combat_atk_count + (int)combat_def_count)) &&
      ('\x03' < combat_def_count)) && (param_1 == 1)) {
    if (_DAT_451b_1efe == 0) {
      uVar6 = FUN_828e_044a(param_2,param_3,0xd12,0x4125);
      iVar5 = (int)(char)DAT_451b_029a;
    }
    else {
      uVar6 = FUN_828e_044a(param_2,param_3,*(char *)((int)_DAT_451b_1efe + 10) * 0x14 + 0x223,
                            0x3c04);
      iVar5 = -1;
    }
    FUN_6d51_1244((int)(char)DAT_451b_029a,5,iVar5,uVar6);
  }
  DAT_451b_1e9e = 0;
  iVar5 = 8;
  while (iVar9 = iVar5 + -1, iVar5 != 0) {
    *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar9 * 4) = 0;
    ((undefined2 *)&DAT_451b_1ede)[iVar9 * 2] = 0;
    iVar5 = iVar9;
  }
  for (iVar5 = 0; iVar9 = DAT_451b_1e9e, iVar5 < combat_atk_count; iVar5 = iVar5 + 1) {
    if (((undefined1 *)&combat_atk_alive)[iVar5] != '\0') {
      uVar6 = ((undefined2 *)&combat_atk_armies)[iVar5 * 2];
      *(undefined2 *)((undefined *)&DAT_451b_1ee0 + DAT_451b_1e9e * 4) =
           ((undefined2 *)&DAT_451b_0368)[iVar5 * 2];
      ((undefined2 *)&DAT_451b_1ede)[iVar9 * 2] = uVar6;
      DAT_451b_1e9e = DAT_451b_1e9e + 1;
    }
  }
  DAT_451b_1f02 = _DAT_451b_1ede;
  for (iVar5 = 0; iVar5 < combat_atk_count; iVar5 = iVar5 + 1) {
    if (((undefined1 *)&combat_atk_alive)[iVar5] == '\0') {
      if (((undefined1 *)&combat_atk_type)[iVar5] == '\x1c') {
        uVar6 = FUN_828e_044a(param_2,param_3,0,
                              *(char *)((int)*(undefined4 *)
                                              ((undefined2 *)&combat_atk_armies + iVar5 * 2) + 10) *
                              0x14 + 0x223,0x3c04);
        FUN_6d51_1244((int)(char)DAT_451b_029a,1,uVar6);
        hero_drop_items((int)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar5 * 2),
                        (int)((ulong)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar5 * 2)
                             >> 0x10),param_2,param_3);
      }
      FUN_1b62_03be((int)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar5 * 2),
                    (int)((ulong)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar5 * 2) >>
                         0x10));
    }
    else if (((undefined1 *)&combat_atk_type)[iVar5] == '\x1c') {
      if (*(char *)(DAT_451b_0292 + 0x710) == '\n') {
        uVar6 = 2;
      }
      else {
        uVar6 = 1;
      }
      hero_add_experience((int)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar5 * 2),
                          (int)((ulong)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar5 * 2)
                               >> 0x10),uVar6);
    }
  }
  for (iVar5 = 0; iVar5 < combat_def_count; iVar5 = iVar5 + 1) {
    if (((undefined1 *)&combat_def_alive)[iVar5] == '\0') {
      if (((undefined1 *)&combat_def_type)[iVar5] == '\x1c') {
        uVar6 = FUN_828e_044a(param_2,param_3,0,
                              *(char *)((int)*(undefined4 *)
                                              ((undefined2 *)&combat_def_armies + iVar5 * 2) + 10) *
                              0x14 + 0x223,0x3c04);
        FUN_6d51_1244((int)(char)DAT_451b_029b,1,uVar6);
        hero_drop_items((int)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar5 * 2),
                        (int)((ulong)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar5 * 2)
                             >> 0x10),param_2,param_3);
      }
      FUN_1b62_03be((int)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar5 * 2),
                    (int)((ulong)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar5 * 2) >>
                         0x10));
    }
    else if (((undefined1 *)&combat_atk_type)[iVar5] == '\x1c') {
      hero_add_experience((int)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar5 * 2),
                          (int)((ulong)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar5 * 2)
                               >> 0x10),1);
    }
  }
  FUN_828e_03f9();
  compute_upkeep();
  compute_income();
  if (param_1 == 0) {
    bVar3 = DAT_451b_029b & 0xf;
    iVar5 = DAT_451b_0281 * 0xe0 + DAT_451b_0289 * 2;
    pbVar1 = (byte *)(iVar5 + 1);
    *pbVar1 = *pbVar1 & 0xf0;
    pbVar1 = (byte *)(iVar5 + 1);
    *pbVar1 = *pbVar1 | bVar3;
    refresh_map_owners();
  }
  else {
    bVar3 = DAT_451b_029a & 0xf;
    iVar5 = DAT_451b_0281 * 0xe0 + DAT_451b_0289 * 2;
    pbVar1 = (byte *)(iVar5 + 1);
    *pbVar1 = *pbVar1 & 0xf0;
    pbVar1 = (byte *)(iVar5 + 1);
    *pbVar1 = *pbVar1 | bVar3;
    FUN_8611_1298(current_player,DAT_451b_0289,DAT_451b_0281);
    FUN_834b_1fed(DAT_451b_0289,DAT_451b_0281,1);
    if ((DAT_451b_0293 & 0x20) != 0) {
      pbVar1 = (byte *)(param_3 * 0xe0 + param_2 * 2 + 1);
      *pbVar1 = *pbVar1 & 0xdf;
    }
    if (*(char *)(DAT_451b_0292 + 0x710) == '\n') {
      if (DAT_451b_029b == 0xf) {
        iVar5 = 0;
      }
      else if ((int)((undefined2 *)&side_city_count)[(char)DAT_451b_029b] < 2) {
        iVar5 = *(int *)((char)DAT_451b_029b * 0x14 + 0x185);
      }
      else {
        iVar5 = *(int *)((char)DAT_451b_029b * 0x14 + 0x185) /
                (int)((undefined2 *)&side_city_count)[(char)DAT_451b_029b];
      }
      if (iVar5 / 2 < 0) {
        iVar5 = 0;
      }
      else {
        iVar5 = iVar5 / 2;
      }
      if ((*(int *)((char)DAT_451b_029a * 2 + 0xd0) == 0) && (0 < iVar5)) {
        iVar9 = iVar5;
        fmt = get_string(0x93,0);
        sprintf((char *)0x41254162,fmt,iVar9);
        FUN_6a35_04c5(0x4162,0x4125);
      }
      if ((char)DAT_451b_029b < '\b') {
        piVar2 = (int *)((char)DAT_451b_029a * 0x14 + 0x185);
        *piVar2 = *piVar2 + iVar5;
        piVar2 = (int *)((char)DAT_451b_029b * 0x14 + 0x185);
        *piVar2 = *piVar2 + iVar5 * -2;
      }
      uVar6 = (undefined2)((ulong)DAT_451b_027d >> 0x10);
      piVar8 = (int *)DAT_451b_027d;
      if (*(char *)((int)piVar8 + 0x15) != '\x0f') {
        if (*(byte *)((int)piVar8 + 0x2f) == current_player) {
          uVar4 = 0xf;
        }
        else {
          uVar4 = *(undefined1 *)((int)piVar8 + 0x15);
        }
        *(undefined1 *)((int)piVar8 + 0x2f) = uVar4;
      }
      uVar6 = (undefined2)((ulong)DAT_451b_027d >> 0x10);
      piVar8 = (int *)DAT_451b_027d;
      *(undefined1 *)(piVar8 + 0x18) = 0;
      *(byte *)((int)piVar8 + 0x15) = DAT_451b_029a;
      *(undefined1 *)((int)piVar8 + 0x2d) = 0;
      iVar5 = 4;
      while (iVar9 = iVar5 + -1, iVar5 != 0) {
        pbVar1 = (byte *)(((undefined2 *)&DAT_451b_0281)[iVar9] * 0xe0 +
                          ((undefined2 *)&DAT_451b_0289)[iVar9] * 2 + 1);
        *pbVar1 = *pbVar1 & 0xef;
        set_city_tiles(DAT_451b_027d,(int)(char)DAT_451b_029a);
        bVar3 = DAT_451b_029a & 0xf;
        iVar5 = ((undefined2 *)&DAT_451b_0281)[iVar9] * 0xe0 +
                ((undefined2 *)&DAT_451b_0289)[iVar9] * 2;
        pbVar1 = (byte *)(iVar5 + 1);
        *pbVar1 = *pbVar1 & 0xf0;
        pbVar1 = (byte *)(iVar5 + 1);
        *pbVar1 = *pbVar1 | bVar3;
        FUN_8611_1298(current_player,((undefined2 *)&DAT_451b_0289)[iVar9],
                      ((undefined2 *)&DAT_451b_0281)[iVar9]);
        FUN_834b_1fed(((undefined2 *)&DAT_451b_0289)[iVar9],((undefined2 *)&DAT_451b_0281)[iVar9],1)
        ;
        iVar5 = iVar9;
      }
      path_build_cost_grid(current_player,1);
      piVar2 = DAT_451b_027d;
      uVar6 = (undefined2)((ulong)DAT_451b_027d >> 0x10);
      piVar8 = (int *)DAT_451b_027d;
      *(undefined2 *)((int)piVar8 + 0x31) = 0;
      uVar7 = FUN_828e_044a(*piVar2,piVar8[1]);
      iVar5 = army_count;
      while (iVar9 = iVar5 + -1, iVar5 != 0) {
        iVar5 = iVar9;
        if (((*(byte *)(iVar9 * 0x16 + -0x777b) == DAT_451b_029b) &&
            (*(int *)(iVar9 * 0x16 + -0x7780) == -1)) &&
           (((*(byte *)(iVar9 * 0x16 + -0x7772) & 0xf) == 1 &&
            (*(byte *)(iVar9 * 0x16 + -0x7771) >> 1 == uVar7)))) {
          FUN_1b62_03be(iVar9 * 0x16 + -0x7780,0x2e1d);
        }
      }
      for (iVar5 = 0; iVar5 < city_count; iVar5 = iVar5 + 1) {
        if (((*(int *)((undefined1 *)&LAB_4125_15ad_1 + iVar5 * 0x41) != 0) &&
            (*(int *)((undefined1 *)&LAB_4125_15b0 + iVar5 * 0x41) == *DAT_451b_027d)) &&
           (*(int *)(iVar5 * 0x41 + 0x15b2) == ((int *)DAT_451b_027d)[1])) {
          *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + iVar5 * 0x41) = 0;
          *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + iVar5 * 0x41) = 0;
          *(undefined1 *)(iVar5 * 0x41 + 0x15aa) = 0;
          *(undefined2 *)(iVar5 * 0x41 + 0x15b2) = 0;
          *(undefined1 *)(iVar5 * 0x41 + 0x15a9) = 0xff;
        }
      }
      FUN_828e_03f9();
      compute_upkeep();
      compute_income();
      return 0x21;
    }
  }
  return 1;
}


