
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far ai_city_garrison_check(uint param_1,int param_2)

{
  byte *pbVar1;
  char *pcVar2;
  int *piVar3;
  uint *puVar4;
  char cVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 unaff_SS;
  int local_ae [8];
  int local_9e [32];
  int local_5e [32];
  int local_1e;
  int local_1c;
  int local_1a;
  uint local_18;
  int local_16;
  int local_14;
  int local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_4 = *(int *)(param_1 * 0x41 + 0x157d);
  local_6 = *(int *)(param_1 * 0x41 + 0x157f);
  local_8 = -1;
  local_a = 0;
  local_c = 0;
  local_e = 0;
  local_10 = 0;
  uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  local_18 = (uint)*(byte *)((int)DAT_451b_018e + param_1 + 0x182);
  local_1a = 0;
  if (((int)param_1 < 0) || (city_count <= (int)param_1)) {
    uVar9 = 0;
  }
  else {
    iVar6 = 8;
    while (iVar6 != 0) {
      local_ae[iVar6 + -1] = -1;
      iVar6 = iVar6 + -1;
    }
    pbVar1 = (byte *)((int)DAT_451b_018e + param_1 + 0x11e);
    *pbVar1 = *pbVar1 | 4;
    iVar6 = 0x20;
    while (iVar8 = iVar6 + -1, iVar6 != 0) {
      local_9e[iVar8] = -1;
      local_5e[iVar8] = -1;
      iVar6 = iVar8;
    }
    *(undefined1 *)((int)DAT_451b_018e + param_1 + 0x182) = 0;
    local_12 = 0;
    iVar6 = army_count;
    while (iVar8 = iVar6 + -1, iVar6 != 0) {
      iVar6 = iVar8;
      if ((((local_4 <= *(int *)(iVar8 * 0x16 + -0x7780)) &&
           (*(int *)(iVar8 * 0x16 + -0x7780) <= local_4 + 1)) &&
          (local_6 <= *(int *)(iVar8 * 0x16 + -0x777e))) &&
         (*(int *)(iVar8 * 0x16 + -0x777e) <= local_6 + 1)) {
        if (*(char *)(iVar8 * 0x16 + -0x777b) != (char)current_player) {
          FUN_5ca7_015e(iVar8);
        }
        pcVar2 = (char *)((int)DAT_451b_018e + param_1 + 0x182);
        *pcVar2 = *pcVar2 + '\x01';
        if (((*(byte *)(iVar8 * 0x16 + -0x7772) & 0xf) == 1) &&
           (*(byte *)(iVar8 * 0x16 + -0x7771) >> 1 == param_1)) {
          pbVar1 = (byte *)(iVar8 * 0x16 + -0x7772);
          *pbVar1 = *pbVar1 & 0xf0;
          pbVar1 = (byte *)(iVar8 * 0x16 + -0x7771);
          *pbVar1 = *pbVar1 & 1;
        }
        if ((*(byte *)(iVar8 * 0x16 + -0x7772) & 0xf) != 0) {
          local_a = local_a + 1;
        }
        if ((*(char *)(iVar8 * 0x16 + -0x777c) == '\x1c') && (local_1a < 8)) {
          local_ae[local_1a] = iVar8;
          local_1a = local_1a + 1;
          local_c = local_c + 1;
          local_8 = iVar8;
        }
        if (((undefined1 *)&army_move_flags)[*(char *)(iVar8 * 0x16 + -0x777c) * 6] != '\0') {
          local_10 = local_10 + 1;
        }
        if (((undefined1 *)&DAT_451b_0668)[*(char *)(iVar8 * 0x16 + -0x777c) * 6] != '\0') {
          local_e = local_e + 1;
        }
        if (local_12 < 0x20) {
          local_5e[local_12] = iVar8;
          local_12 = local_12 + 1;
        }
        else {
          FUN_5ca7_015e(iVar8);
        }
      }
    }
    if (local_8 != -1) {
      DAT_451b_1efe = local_8 * 0x16 + -0x7780;
      _DAT_451b_1f00 = 0x2e1d;
      FUN_6087_0427(*(undefined2 *)(local_8 * 0x16 + -0x7780));
    }
    if (1 < local_c) {
      FUN_6087_17db(local_ae);
    }
    uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    pbVar1 = (byte *)((int)DAT_451b_018e + param_1 + 0x11e);
    *pbVar1 = *pbVar1 & 0xcf;
    if (local_c != 0) {
      pbVar1 = (byte *)((int)DAT_451b_018e + param_1 + 0x11e);
      *pbVar1 = *pbVar1 | 0x20;
    }
    if (local_e != 0) {
      pbVar1 = (byte *)((int)DAT_451b_018e + param_1 + 0x11e);
      *pbVar1 = *pbVar1 | 0x10;
    }
    if (0x10 < local_12) {
      local_18 = 0;
      local_a = 0;
    }
    uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (((local_a < 3) || (*(char *)((int)DAT_451b_018e + param_1 + 0x56) == '\a')) &&
       ((param_2 != 0 ||
        ((((local_c != 0 || (*(byte *)((int)DAT_451b_018e + param_1 + 0x182) != local_18)) ||
          (iVar6 = dice(1,6,-1), iVar6 == 0)) ||
         ((*(byte *)(local_6 * 0xe0 + local_4 * 2 + 0xe3) & 0x10) != 0)))))) {
      uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      *(undefined1 *)((int)DAT_451b_018e + param_1 + 0x1e6) = 0;
      cVar5 = *(char *)((int)DAT_451b_018e + param_1 + 0x56);
      if (((cVar5 == '\x04') || (cVar5 == '\x05')) || (cVar5 == '\b')) {
        local_1c = ai_wanted_garrison(param_1);
        iVar6 = (int)DAT_451b_018e;
        uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        if (local_12 < 2) {
          *(undefined1 *)(iVar6 + param_1 + 0x56) = 4;
        }
        else if (local_12 < local_1c) {
          *(undefined1 *)(iVar6 + param_1 + 0x56) = 5;
        }
        else {
          *(undefined1 *)(iVar6 + param_1 + 0x56) = 8;
        }
      }
      local_14 = FUN_5ca7_0b5c(param_1,local_12,local_5e);
      if (((param_2 == 0) && (*(int *)(current_player * 0x14 + 0x185) < 300)) &&
         ((int)((undefined2 *)&side_income)[current_player] <
          ((undefined2 *)&side_upkeep)[current_player] * 2)) {
        uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        if ((((*(char *)((int)DAT_451b_018e + param_1 + 0x56) != '\a') &&
             (10 < *(byte *)((int)DAT_451b_018e + param_1 + 0xba))) &&
            (local_c + local_e + 4 < local_12)) &&
           (iVar6 = FUN_57ea_0a3e(*(undefined2 *)(param_1 * 0x41 + 0x157d),
                                  *(undefined2 *)(param_1 * 0x41 + 0x157f),0xffff), 10 < iVar6)) {
          for (iVar6 = local_c + local_e + 4; iVar6 < local_12; iVar6 = iVar6 + 1) {
            iVar8 = local_9e[iVar6];
            if (iVar8 != -1) {
              local_9e[iVar6] = -1;
              FUN_5ca7_015e(iVar8,0);
            }
          }
        }
      }
      if (0x18 < local_12) {
        for (iVar6 = 0x18; iVar6 < local_12; iVar6 = iVar6 + 1) {
          iVar8 = local_9e[iVar6];
          if (iVar8 != -1) {
            local_9e[iVar6] = -1;
            FUN_5ca7_015e(iVar8,0);
          }
        }
      }
      if (((opt_quick_start == 0) || (iVar6 = game_turn(), 2 < iVar6)) && (local_12 < 2)) {
        *(undefined1 *)((int)DAT_451b_018e + param_1 + 0x56) = 4;
      }
      iVar6 = 4;
      while (iVar8 = iVar6 + -1, iVar6 != 0) {
        pbVar1 = (byte *)((local_6 + *(int *)(iVar8 * 2 + 0x81c)) * 0xe0 +
                          (local_4 + *(int *)(iVar8 * 2 + 0x814)) * 2 + 1);
        *pbVar1 = *pbVar1 & 0xef;
        iVar6 = iVar8;
      }
      if (opt_hidden_map != 0) {
        uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        if ((*(char *)((int)DAT_451b_018e + param_1 + 0x56) == '\v') &&
           (*(int *)((int)DAT_451b_018e + 0x24) < 5)) {
          local_1c = local_10;
          iVar6 = 2 - (local_12 - local_10);
          if (0 < iVar6) {
            local_1c = local_10 - iVar6;
          }
          if ((opt_quick_start != 0) && (iVar6 = game_turn(), iVar6 < 3)) {
            local_1c = 1;
          }
          for (iVar6 = 0; iVar6 < local_12; iVar6 = iVar6 + 1) {
            if (((0 < local_1c) && (iVar8 = local_9e[iVar6], iVar8 != -1)) &&
               (((undefined1 *)&army_move_flags)[*(char *)(iVar8 * 0x16 + -0x777c) * 6] != '\0')) {
              piVar3 = (int *)((int)DAT_451b_018e + 0x24);
              *piVar3 = *piVar3 + 1;
              puVar4 = (uint *)(iVar8 * 0x16 + -0x7774);
              *puVar4 = *puVar4 | 0x20;
              FUN_57ea_0e4b(iVar8,0xffff);
              local_9e[iVar6] = -1;
              local_1c = local_1c + -1;
            }
          }
        }
      }
      uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      if ((*(char *)((int)DAT_451b_018e + param_1 + 0x56) == '\x03') ||
         (*(char *)((int)DAT_451b_018e + param_1 + 0x56) == '\x02')) {
        uVar7 = 1;
        local_16 = 8;
      }
      else {
        uVar7 = (uint)(local_14 == 0);
        local_16 = local_14;
        if (local_14 == 0) {
          local_16 = 8;
        }
      }
      for (iVar6 = 0; iVar6 < local_12; iVar6 = iVar6 + 1) {
        iVar8 = local_9e[iVar6];
        if (iVar8 != -1) {
          if (uVar7 == 0) {
            pcVar2 = (char *)((int)DAT_451b_018e + param_1 + 0x1e6);
            *pcVar2 = *pcVar2 + '\x01';
          }
          local_9e[iVar6] = -1;
          local_1c = *(int *)(param_1 * 0x41 + 0x157d) + *(int *)(uVar7 * 2 + 0x814);
          local_1e = *(int *)(param_1 * 0x41 + 0x157f) + *(int *)(uVar7 * 2 + 0x81c);
          *(int *)(iVar8 * 0x16 + -0x7780) = local_1c;
          *(int *)(iVar8 * 0x16 + -0x777e) = local_1e;
          *(undefined2 *)(iVar8 * 0x16 + -0x776c) = 0xffff;
          *(undefined2 *)(iVar8 * 0x16 + -0x776e) = 0xffff;
          *(undefined1 *)(iVar8 * 0x16 + -0x776f) = 0;
          pbVar1 = (byte *)(iVar8 * 0x16 + -0x7772);
          *pbVar1 = *pbVar1 & 0x8f;
          *(undefined1 *)(iVar8 * 0x16 + -0x7770) = (undefined1)param_1;
          puVar4 = (uint *)(iVar8 * 0x16 + -0x7774);
          *puVar4 = *puVar4 & 0xffbf;
          pbVar1 = (byte *)(local_1e * 0xe0 + local_1c * 2 + 1);
          *pbVar1 = *pbVar1 | 0x10;
          local_16 = local_16 + -1;
          if (local_16 == 0) {
            uVar7 = uVar7 + 1;
            local_16 = 8;
          }
        }
      }
      FUN_5ca7_10a1(*(undefined2 *)(param_1 * 0x41 + 0x157d),
                    *(undefined2 *)(param_1 * 0x41 + 0x157f));
    }
    uVar9 = 1;
  }
  return uVar9;
}


