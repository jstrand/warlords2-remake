
void __cdecl16far ai_phase_move_hero(void)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  undefined2 unaff_SS;
  undefined1 local_5a [16];
  int local_4a [6];
  int local_3e [20];
  uint local_16;
  int local_a;
  int local_8;
  undefined2 local_6;
  int local_4;
  
  local_6 = 0;
  iVar5 = 4;
  while( true ) {
    iVar6 = iVar5 + -1;
    uVar7 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (iVar5 == 0) break;
    iVar5 = iVar6;
    if (*(int *)((int)DAT_451b_018e + iVar6 * 0x5c + 0x24c) != 0) {
      local_6 = 1;
    }
  }
  *(undefined2 *)((int)DAT_451b_018e + 0x46) = 0xffff;
  if (((opt_quests != 0) && (*(int *)(current_player * 0xc + 0x1103) != 0)) &&
     ((*(int *)(current_player * 0xc + 0x1105) == 4 ||
      (*(int *)(current_player * 0xc + 0x1105) == 5)))) {
    *(undefined2 *)((int)DAT_451b_018e + 0x46) = *(undefined2 *)(current_player * 0xc + 0x1109);
  }
  FUN_6087_065d(local_4a);
  iVar5 = 6;
  do {
    do {
      do {
        iVar6 = iVar5 + -1;
        if (iVar5 == 0) {
          return;
        }
        iVar2 = local_4a[iVar6];
        iVar5 = iVar6;
      } while (((iVar2 == -1) || (*(char *)(iVar2 * 0x16 + -0x777b) != current_player)) ||
              (*(char *)(iVar2 * 0x16 + -0x777c) != '\x1c'));
      if (-1 < local_3e[iVar6]) {
        ai_city_garrison_check(local_3e[iVar6],1);
      }
    } while (((opt_quests != 0) && (*(int *)(current_player * 0xc + 0x1103) != 0)) &&
            ((*(int *)(current_player * 0xc + 0x1107) == iVar2 &&
             (iVar3 = auto_str_quest_city_going_dist(iVar6,local_4a), iVar3 != 0))));
    if ((*(byte *)(iVar2 * 0x16 + -0x7772) & 0xf) == 3) {
      iVar4 = FUN_6087_1637(*(byte *)(iVar2 * 0x16 + -0x7771) >> 1,iVar2);
      iVar3 = army_count;
      if (iVar4 == 0) {
        while (iVar4 = iVar3 + -1, iVar3 != 0) {
          iVar3 = iVar4;
          if (((*(char *)(iVar4 * 0x16 + -0x777b) == current_player) &&
              (*(int *)(iVar4 * 0x16 + -0x7780) == *(int *)(iVar2 * 0x16 + -0x7780))) &&
             ((*(int *)(iVar4 * 0x16 + -0x777e) == *(int *)(iVar2 * 0x16 + -0x777e) &&
              ((*(byte *)(iVar4 * 0x16 + -0x7772) & 0xf) ==
               (*(byte *)(iVar2 * 0x16 + -0x7772) & 0xf))))) {
            pbVar1 = (byte *)(iVar4 * 0x16 + -0x7772);
            *pbVar1 = *pbVar1 & 0xf0;
            pbVar1 = (byte *)(iVar4 * 0x16 + -0x7771);
            *pbVar1 = *pbVar1 & 1;
            *(undefined2 *)(iVar4 * 0x16 + -0x776c) = 0xffff;
            *(undefined2 *)(iVar4 * 0x16 + -0x776e) = 0xffff;
          }
        }
      }
      else {
        local_16 = (uint)(*(byte *)(iVar2 * 0x16 + -0x7771) >> 1);
        auto_str_scan_dist_status_bonus(iVar6,3,local_4a);
      }
    }
    iVar3 = 1;
    local_a = 0;
    local_4 = 0;
    while (((iVar3 != 0 && (local_a < 2)) && ('\x03' < *(char *)(iVar2 * 0x16 + -0x7779)))) {
      local_a = local_a + 1;
      auto_str_legal_site_dist_current(iVar6,local_4a);
      FUN_6087_11f8(iVar6,local_4a);
      auto_str_enemy_city_combat_dist(iVar6,local_4a);
      local_8 = ai_choose_target(iVar6,local_4a);
      if (local_8 - 1U < 4) {
                    /* WARNING: Switch is manually overridden */
        switch((undefined4)*(undefined2 *)((local_8 - 1U) * 2 + 0x41f)) {
        case 0x60b99:
          goto LAB_6087_03c0;
        case 0x60b9c:
          local_4 = 1;
          iVar3 = auto_str_scan_dist_status_bonus(iVar6,local_8,local_4a);
          break;
        case 0x60bb8:
          local_4 = 1;
          iVar3 = FUN_6087_0b75(iVar6,local_4a);
        }
      }
      else {
        if (((local_4 != 0) && (opt_hidden_map != 0)) &&
           ((iVar3 = game_turn(), iVar3 < 10 &&
            (iVar3 = FUN_623c_12c9(*(undefined2 *)(iVar2 * 0x16 + -0x7780),
                                   *(undefined2 *)(iVar2 * 0x16 + -0x777e),local_5a), iVar3 == 1))))
        {
          FUN_57ea_0e4b(iVar2,0xffff);
        }
LAB_6087_03c0:
        iVar3 = 0;
      }
      if ((*(char *)(iVar2 * 0x16 + -0x777b) != current_player) ||
         (*(char *)(iVar2 * 0x16 + -0x777c) != '\x1c')) {
        iVar3 = 0;
      }
    }
  } while( true );
}


