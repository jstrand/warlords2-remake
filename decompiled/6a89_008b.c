
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far combat_setup(int x,int y)

{
  bool bVar1;
  char cVar2;
  undefined1 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 unaff_SS;
  char local_92 [32];
  undefined1 local_72 [32];
  int local_52;
  int local_50;
  int local_4e;
  int local_4c;
  int local_4a;
  int local_48;
  int local_46;
  int local_44;
  int local_40;
  int local_3e;
  char local_35 [9];
  int local_2c;
  int local_2a;
  int local_28;
  int local_26;
  int local_24;
  int local_22;
  int local_20;
  int local_1e;
  int local_1c;
  int local_1a;
  int local_18;
  int local_16;
  int local_14;
  int local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  local_e = 0;
  local_12 = 0;
  local_14 = 0;
  local_18 = 0;
  local_1a = 0;
  bVar1 = false;
  local_1e = 0;
  local_20 = 0;
  local_22 = 0;
  local_24 = 0;
  local_26 = 0;
  local_28 = 0;
  local_2a = 0;
  local_2c = combat_terrain_class(x,y);
  DAT_451b_029a = (char)current_player;
  DAT_451b_029b = *(byte *)(y * 0xe0 + x * 2 + 1) & 0xf;
  if (DAT_451b_029b < 8) {
    local_c = (int)(char)DAT_451b_029b;
  }
  else {
    local_c = 8;
  }
  if (*(char *)(*(byte *)(y * 0xe0 + x * 2) + 0x710) == '\n') {
    DAT_451b_0291 = '\x04';
    iVar7 = 0;
    while ((iVar7 < city_count &&
           (((*(int *)(iVar7 * 0x41 + 0x157d) != x && (*(int *)(iVar7 * 0x41 + 0x157d) != x + -1))
            || ((*(int *)(iVar7 * 0x41 + 0x157f) != y && (*(int *)(iVar7 * 0x41 + 0x157f) != y + -1)
                )))))) {
      iVar7 = iVar7 + 1;
    }
    iVar6 = 4;
    while (iVar4 = iVar6 + -1, iVar6 != 0) {
      *(undefined2 *)((undefined1 *)&DAT_451b_0292 + iVar4 * 2) =
           *(undefined2 *)
            ((*(int *)(iVar7 * 0x41 + 0x157f) + *(int *)(iVar4 * 2 + 0xd4e)) * 0xe0 +
            (*(int *)(iVar7 * 0x41 + 0x157d) + *(int *)(iVar4 * 2 + 0xd46)) * 2);
      ((undefined2 *)&DAT_451b_0289)[iVar4] =
           *(int *)(iVar7 * 0x41 + 0x157d) + *(int *)(iVar4 * 2 + 0xd46);
      ((undefined2 *)&DAT_451b_0281)[iVar4] =
           *(int *)(iVar7 * 0x41 + 0x157f) + *(int *)(iVar4 * 2 + 0xd4e);
      iVar6 = iVar4;
    }
    DAT_451b_027d = CONCAT22(0x3c04,iVar7 * 0x41 + 0x157d);
  }
  else {
    DAT_451b_0291 = '\x01';
    _DAT_451b_0292 = *(uint *)(y * 0xe0 + x * 2);
    DAT_451b_0289 = x;
    DAT_451b_0281 = y;
    DAT_451b_027d = 0;
  }
  combat_atk_count = (char)DAT_451b_1e9e;
  if (DAT_4125_2bda == 0) {
    combat_atk_count = '\x01';
  }
  for (iVar7 = 0; iVar7 < combat_atk_count; iVar7 = iVar7 + 1) {
    ((undefined1 *)&combat_atk_alive)[iVar7] = 1;
  }
  if (DAT_4125_2bda == 0) {
    _combat_atk_armies = DAT_451b_1f02;
  }
  else {
    for (iVar7 = 0; iVar7 < combat_atk_count; iVar7 = iVar7 + 1) {
      uVar8 = ((undefined2 *)&DAT_451b_1ede)[iVar7 * 2];
      ((undefined2 *)&DAT_451b_0368)[iVar7 * 2] =
           *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar7 * 4);
      ((undefined2 *)&combat_atk_armies)[iVar7 * 2] = uVar8;
    }
  }
  combat_def_count = '\0';
  iVar7 = army_count;
  if (DAT_451b_0291 == '\x04') {
    while (iVar6 = iVar7 + -1, iVar7 != 0) {
      iVar7 = iVar6;
      if ((((*(char *)(iVar6 * 0x16 + -0x777b) != (char)current_player) &&
           (DAT_451b_0289 <= *(int *)(iVar6 * 0x16 + -0x7780))) &&
          (*(int *)(iVar6 * 0x16 + -0x7780) <= DAT_451b_0289 + 1)) &&
         ((DAT_451b_0281 <= *(int *)(iVar6 * 0x16 + -0x777e) &&
          (*(int *)(iVar6 * 0x16 + -0x777e) <= DAT_451b_0281 + 1)))) {
        iVar4 = (int)combat_def_count;
        *(undefined2 *)((undefined *)&DAT_451b_0388 + iVar4 * 4) = 0x2e1d;
        ((undefined2 *)&combat_def_armies)[iVar4 * 2] = iVar6 * 0x16 + -0x7780;
        combat_def_count = combat_def_count + '\x01';
      }
    }
  }
  else {
    while (iVar6 = iVar7 + -1, iVar7 != 0) {
      iVar7 = iVar6;
      if (((*(char *)(iVar6 * 0x16 + -0x777b) != (char)current_player) &&
          (*(int *)(iVar6 * 0x16 + -0x7780) == DAT_451b_0289)) &&
         (*(int *)(iVar6 * 0x16 + -0x777e) == DAT_451b_0281)) {
        iVar4 = (int)combat_def_count;
        *(undefined2 *)((undefined *)&DAT_451b_0388 + iVar4 * 4) = 0x2e1d;
        ((undefined2 *)&combat_def_armies)[iVar4 * 2] = iVar6 * 0x16 + -0x7780;
        combat_def_count = combat_def_count + '\x01';
      }
    }
  }
  for (iVar7 = 0; iVar7 < combat_def_count; iVar7 = iVar7 + 1) {
    ((undefined1 *)&combat_def_alive)[iVar7] = 1;
  }
  if (((*(char *)(*(byte *)(y * 0xe0 + x * 2) + 0x710) == '\x02') ||
      (*(char *)(*(byte *)(y * 0xe0 + x * 2) + 0x710) == '\x03')) ||
     (*(char *)(*(byte *)(y * 0xe0 + x * 2) + 0x710) == '\x06')) {
    bVar1 = true;
  }
  for (iVar7 = 0; iVar7 < combat_def_count; iVar7 = iVar7 + 1) {
    local_92[iVar7] =
         *(char *)(local_c * 0x1d +
                   (int)*(char *)((int)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar7 * 2)
                                 + 4) + 0x60b);
    if ((local_28 == 0) &&
       (*(char *)((int)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar7 * 2) + 4) == '\x1c')
       ) {
      local_28 = 1;
    }
  }
  for (iVar7 = 0; iVar7 < combat_atk_count; iVar7 = iVar7 + 1) {
    local_35[iVar7 + 1] =
         *(char *)(DAT_451b_029a * 0x1d +
                   (int)*(char *)((int)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar7 * 2)
                                 + 4) + 0x60b);
    if ((local_2a == 0) &&
       (*(char *)((int)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar7 * 2) + 4) == '\x1c')
       ) {
      local_2a = 1;
    }
  }
  for (iVar7 = 0; iVar7 < combat_def_count; iVar7 = iVar7 + 1) {
    if ((((bVar1) &&
         (uVar8 = (undefined2)
                  ((ulong)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar7 * 2) >> 0x10),
         (*(uint *)((int)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar7 * 2) + 0xc) &
         0x1000) == 0)) &&
        (((undefined1 *)&army_move_flags)
         [*(char *)(((undefined2 *)&combat_def_armies)[iVar7 * 2] + 4) * 6] != '\0')) &&
       (local_28 != 0)) {
      local_92[iVar7] = local_92[iVar7] + 'P';
      break;
    }
  }
  iVar7 = 0;
  do {
    if (combat_atk_count <= iVar7) {
LAB_6a89_05f1:
      for (iVar7 = 1; iVar7 < combat_atk_count; iVar7 = iVar7 + 1) {
        iVar6 = iVar7;
        if (local_35[iVar7 + 1] < local_35[iVar7]) {
          for (; (0 < iVar6 && (local_35[iVar6 + 1] < local_35[iVar6])); iVar6 = iVar6 + -1) {
            local_4 = ((undefined2 *)&DAT_451b_0368)[iVar6 * 2];
            local_6 = ((undefined2 *)&combat_atk_armies)[iVar6 * 2];
            uVar8 = *(undefined2 *)(iVar6 * 4 + 0x42c2);
            ((undefined2 *)&DAT_451b_0368)[iVar6 * 2] =
                 *(undefined2 *)((undefined1 *)&combat_atk_count + iVar6 * 4);
            ((undefined2 *)&combat_atk_armies)[iVar6 * 2] = uVar8;
            *(undefined2 *)((undefined1 *)&combat_atk_count + iVar6 * 4) = local_4;
            *(undefined2 *)(iVar6 * 4 + 0x42c2) = local_6;
            cVar2 = local_35[iVar6 + 1];
            local_35[iVar6 + 1] = local_35[iVar6];
            local_35[iVar6] = cVar2;
          }
        }
      }
      for (iVar7 = 1; iVar7 < combat_def_count; iVar7 = iVar7 + 1) {
        iVar6 = iVar7;
        if (local_92[iVar7] < local_92[iVar7 + -1]) {
          for (; (0 < iVar6 && (local_92[iVar6] < local_92[iVar6 + -1])); iVar6 = iVar6 + -1) {
            local_4 = *(undefined2 *)((undefined *)&DAT_451b_0388 + iVar6 * 4);
            local_6 = ((undefined2 *)&combat_def_armies)[iVar6 * 2];
            uVar8 = *(undefined2 *)((undefined4 *)&DAT_451b_0382 + iVar6);
            *(undefined2 *)((undefined *)&DAT_451b_0388 + iVar6 * 4) =
                 *(undefined2 *)((int)(undefined4 *)&DAT_451b_0382 + iVar6 * 4 + 2);
            ((undefined2 *)&combat_def_armies)[iVar6 * 2] = uVar8;
            *(undefined2 *)((int)(undefined4 *)&DAT_451b_0382 + iVar6 * 4 + 2) = local_4;
            *(undefined2 *)((undefined4 *)&DAT_451b_0382 + iVar6) = local_6;
            cVar2 = local_92[iVar6];
            local_92[iVar6] = local_92[iVar6 + -1];
            local_92[iVar6 + -1] = cVar2;
          }
        }
      }
      iVar7 = (int)combat_atk_count;
      while (iVar7 != 0) {
        *(undefined1 *)(iVar7 + 0x4273) = 1;
        *(undefined1 *)(iVar7 + 0x424b) =
             *(undefined1 *)
              ((int)*(undefined4 *)((undefined2 *)&combat_atk_armies + (iVar7 + -1) * 2) + 4);
        iVar7 = iVar7 + -1;
      }
      iVar7 = (int)combat_def_count;
      while (iVar7 != 0) {
        ((undefined1 *)&DAT_451b_031b)[iVar7] = 1;
        ((undefined1 *)&DAT_451b_02f3)[iVar7] =
             *(undefined1 *)
              ((int)*(undefined4 *)((undefined2 *)&combat_def_armies + (iVar7 + -1) * 2) + 4);
        iVar7 = iVar7 + -1;
      }
      iVar7 = 0;
      local_10 = 0;
      for (iVar6 = 0; iVar6 < combat_atk_count; iVar6 = iVar6 + 1) {
        armytype_lookup((int)(char)((undefined1 *)&combat_atk_type)[iVar6],local_72);
        if (((undefined1 *)&combat_atk_type)[iVar6] == '\x1c') {
          iVar4 = (int)*(char *)((int)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar6 * 2)
                                + 8);
          iVar5 = hero_battle_items((int)*(undefined4 *)
                                          ((undefined2 *)&combat_atk_armies + iVar6 * 2),
                                    (int)((ulong)*(undefined4 *)
                                                  ((undefined2 *)&combat_atk_armies + iVar6 * 2) >>
                                         0x10));
          iVar4 = iVar4 + iVar5;
          if ((iVar7 < iVar4) && (iVar7 = iVar4, 9 < iVar4)) {
            iVar7 = 9;
          }
        }
        if (local_3e == 1) {
          local_1e = 1;
        }
        if (local_3e == 2) {
          local_20 = 1;
        }
        if (local_3e == 3) {
          local_22 = 1;
        }
        iVar4 = local_1a;
        if ((local_40 != 0) && (iVar4 = local_40, local_1a < local_40)) {
          iVar4 = local_1a;
        }
        local_1a = iVar4;
        iVar4 = local_12;
        if (((local_2c == 0) && (local_4a != 0)) && (iVar4 = local_4a, local_4a < local_12)) {
          iVar4 = local_12;
        }
        local_12 = iVar4;
        iVar4 = local_12;
        if (((local_2c == 1) && (local_48 != 0)) && (iVar4 = local_48, local_48 < local_12)) {
          iVar4 = local_12;
        }
        local_12 = iVar4;
        iVar4 = local_12;
        if (((local_2c == 2) && (local_46 != 0)) && (iVar4 = local_46, local_46 < local_12)) {
          iVar4 = local_12;
        }
        local_12 = iVar4;
        iVar4 = local_12;
        if (((local_2c == 3) && (local_44 != 0)) && (iVar4 = local_44, local_44 < local_12)) {
          iVar4 = local_12;
        }
        local_12 = iVar4;
        if (((undefined1 *)&combat_atk_type)[iVar6] == '\x1c') {
          iVar4 = hero_command_items((int)*(undefined4 *)
                                           ((undefined2 *)&combat_atk_armies + iVar6 * 2),
                                     (int)((ulong)*(undefined4 *)
                                                   ((undefined2 *)&combat_atk_armies + iVar6 * 2) >>
                                          0x10));
          local_10 = local_10 + iVar4;
        }
      }
      local_16 = local_10 + *(int *)(iVar7 * 2 + 0xce4);
      iVar7 = 0;
      local_10 = 0;
      for (iVar6 = 0; iVar6 < combat_def_count; iVar6 = iVar6 + 1) {
        armytype_lookup((int)(char)((undefined1 *)&combat_def_type)[iVar6],local_72);
        if (((undefined1 *)&combat_def_type)[iVar6] == '\x1c') {
          iVar4 = (int)*(char *)((int)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar6 * 2)
                                + 8);
          iVar5 = hero_battle_items((int)*(undefined4 *)
                                          ((undefined2 *)&combat_def_armies + iVar6 * 2),
                                    (int)((ulong)*(undefined4 *)
                                                  ((undefined2 *)&combat_def_armies + iVar6 * 2) >>
                                         0x10));
          iVar4 = iVar4 + iVar5;
          if ((iVar7 < iVar4) && (iVar7 = iVar4, 9 < iVar4)) {
            iVar7 = 9;
          }
        }
        if (local_3e == 2) {
          local_24 = 1;
        }
        if (local_3e == 3) {
          local_26 = 1;
        }
        iVar4 = local_14;
        if ((local_40 != 0) && (iVar4 = local_40, local_14 < local_40)) {
          iVar4 = local_14;
        }
        local_14 = iVar4;
        iVar4 = local_18;
        if (((local_2c == 0) && (local_4a != 0)) && (iVar4 = local_4a, local_4a < local_18)) {
          iVar4 = local_18;
        }
        local_18 = iVar4;
        iVar4 = local_18;
        if (((local_2c == 1) && (local_48 != 0)) && (iVar4 = local_48, local_48 < local_18)) {
          iVar4 = local_18;
        }
        local_18 = iVar4;
        iVar4 = local_18;
        if (((local_2c == 2) && (local_46 != 0)) && (iVar4 = local_46, local_46 < local_18)) {
          iVar4 = local_18;
        }
        local_18 = iVar4;
        iVar4 = local_18;
        if (((local_2c == 3) && (local_44 != 0)) && (iVar4 = local_44, local_44 < local_18)) {
          iVar4 = local_18;
        }
        local_18 = iVar4;
        if (((undefined1 *)&combat_def_type)[iVar6] == '\x1c') {
          iVar4 = hero_command_items((int)*(undefined4 *)
                                           ((undefined2 *)&combat_def_armies + iVar6 * 2),
                                     (int)((ulong)*(undefined4 *)
                                                   ((undefined2 *)&combat_def_armies + iVar6 * 2) >>
                                          0x10));
          local_10 = local_10 + iVar4;
        }
      }
      if ((local_1e == 0) && (local_2c == 0)) {
        if ((_DAT_451b_0292 >> 8 & 0x20) == 0) {
          if (*(char *)(*(byte *)(y * 0xe0 + x * 2) + 0x710) == '\v') {
            local_e = 2;
          }
          else {
            local_e = (int)*(char *)((int)DAT_451b_027d + 0x14);
          }
        }
        else {
          local_e = 1;
        }
        if (DAT_451b_029b == 0xf) {
          local_e = local_e / 2;
        }
      }
      local_1c = local_10 + *(int *)(iVar7 * 2 + 0xce4);
      local_8 = 0;
      if (local_24 == 0) {
        local_8 = local_16;
      }
      if (local_26 == 0) {
        local_8 = local_8 + local_12;
      }
      if (combat_modifier_cap < local_8) {
        local_8 = combat_modifier_cap;
      }
      local_8 = local_8 + local_14;
      local_a = 0;
      if (local_20 == 0) {
        local_a = local_1c;
      }
      if (local_22 == 0) {
        local_a = local_a + local_18;
      }
      if (local_1e == 0) {
        local_a = local_a + local_e;
      }
      if (combat_modifier_cap < local_a) {
        local_a = combat_modifier_cap;
      }
      local_a = local_a + local_1a;
      for (iVar7 = 0; iVar7 < combat_atk_count; iVar7 = iVar7 + 1) {
        armytype_lookup((int)(char)((undefined1 *)&combat_atk_type)[iVar7],local_72);
        ((undefined1 *)&DAT_451b_029c)[iVar7] = 0;
        if (((*(uint *)((int)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar7 * 2) + 0xc) &
             0x1000) == 0) ||
           ((*(char *)(*(byte *)(y * 0xe0 + x * 2) + 0x710) != '\x02' &&
            (*(char *)(*(byte *)(y * 0xe0 + x * 2) + 0x710) != '\x03')))) {
          ((undefined1 *)&combat_atk_str)[iVar7] =
               *(char *)((int)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar7 * 2) + 8) +
               (char)local_8;
          if (((undefined1 *)&combat_atk_type)[iVar7] == '\x1c') {
            cVar2 = hero_battle_items((int)*(undefined4 *)
                                            ((undefined2 *)&combat_atk_armies + iVar7 * 2),
                                      (int)((ulong)*(undefined4 *)
                                                    ((undefined2 *)&combat_atk_armies + iVar7 * 2)
                                           >> 0x10));
            ((undefined1 *)&combat_atk_str)[iVar7] = ((undefined1 *)&combat_atk_str)[iVar7] + cVar2;
          }
          if ((local_2c == 0) && (local_52 != 0)) {
            ((undefined1 *)&combat_atk_str)[iVar7] =
                 ((undefined1 *)&combat_atk_str)[iVar7] + (char)local_52;
          }
          if ((local_2c == 1) && (local_50 != 0)) {
            ((undefined1 *)&combat_atk_str)[iVar7] =
                 ((undefined1 *)&combat_atk_str)[iVar7] + (char)local_50;
          }
          if ((local_2c == 2) && (local_4e != 0)) {
            ((undefined1 *)&combat_atk_str)[iVar7] =
                 ((undefined1 *)&combat_atk_str)[iVar7] + (char)local_4e;
          }
          if ((local_2c == 3) && (local_4c != 0)) {
            ((undefined1 *)&combat_atk_str)[iVar7] =
                 ((undefined1 *)&combat_atk_str)[iVar7] + (char)local_4c;
          }
          if ((char)((undefined1 *)&combat_atk_str)[iVar7] < '\x10') {
            uVar3 = ((undefined1 *)&combat_atk_str)[iVar7];
          }
          else {
            uVar3 = 0xf;
          }
          ((undefined1 *)&combat_atk_str)[iVar7] = uVar3;
        }
        else {
          ((undefined1 *)&combat_atk_str)[iVar7] = 4;
          ((undefined1 *)&DAT_451b_029c)[iVar7] = 1;
        }
      }
      for (iVar7 = 0; iVar7 < combat_def_count; iVar7 = iVar7 + 1) {
        armytype_lookup((int)(char)((undefined1 *)&combat_def_type)[iVar7],local_72);
        ((undefined1 *)&DAT_451b_02a4)[iVar7] = 0;
        if (((*(uint *)((int)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar7 * 2) + 0xc) &
             0x1000) == 0) ||
           ((*(char *)(*(byte *)(y * 0xe0 + x * 2) + 0x710) != '\x02' &&
            (*(char *)(*(byte *)(y * 0xe0 + x * 2) + 0x710) != '\x03')))) {
          ((undefined1 *)&combat_def_str)[iVar7] =
               *(char *)((int)*(undefined4 *)((undefined2 *)&combat_def_armies + iVar7 * 2) + 8) +
               (char)local_a;
          if (((undefined1 *)&combat_def_type)[iVar7] == '\x1c') {
            cVar2 = hero_battle_items((int)*(undefined4 *)
                                            ((undefined2 *)&combat_def_armies + iVar7 * 2),
                                      (int)((ulong)*(undefined4 *)
                                                    ((undefined2 *)&combat_def_armies + iVar7 * 2)
                                           >> 0x10));
            ((undefined1 *)&combat_def_str)[iVar7] = ((undefined1 *)&combat_def_str)[iVar7] + cVar2;
          }
          if ((local_2c == 0) && (local_52 != 0)) {
            ((undefined1 *)&combat_def_str)[iVar7] =
                 ((undefined1 *)&combat_def_str)[iVar7] + (char)local_52;
          }
          if ((local_2c == 1) && (local_50 != 0)) {
            ((undefined1 *)&combat_def_str)[iVar7] =
                 ((undefined1 *)&combat_def_str)[iVar7] + (char)local_50;
          }
          if ((local_2c == 2) && (local_4e != 0)) {
            ((undefined1 *)&combat_def_str)[iVar7] =
                 ((undefined1 *)&combat_def_str)[iVar7] + (char)local_4e;
          }
          if ((local_2c == 3) && (local_4c != 0)) {
            ((undefined1 *)&combat_def_str)[iVar7] =
                 ((undefined1 *)&combat_def_str)[iVar7] + (char)local_4c;
          }
          if ((char)((undefined1 *)&combat_def_str)[iVar7] < '\x10') {
            uVar3 = ((undefined1 *)&combat_def_str)[iVar7];
          }
          else {
            uVar3 = 0xf;
          }
          ((undefined1 *)&combat_def_str)[iVar7] = uVar3;
        }
        else {
          ((undefined1 *)&combat_def_str)[iVar7] = 4;
          ((undefined1 *)&DAT_451b_02a4)[iVar7] = 1;
        }
      }
      return;
    }
    if (((bVar1) &&
        (uVar8 = (undefined2)
                 ((ulong)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar7 * 2) >> 0x10),
        (*(uint *)((int)*(undefined4 *)((undefined2 *)&combat_atk_armies + iVar7 * 2) + 0xc) &
        0x1000) == 0)) &&
       ((((undefined1 *)&army_move_flags)
         [*(char *)(((undefined2 *)&combat_atk_armies)[iVar7 * 2] + 4) * 6] != '\0' &&
        (local_2a != 0)))) {
      local_35[iVar7 + 1] = local_35[iVar7 + 1] + 'P';
      goto LAB_6a89_05f1;
    }
    iVar7 = iVar7 + 1;
  } while( true );
}


