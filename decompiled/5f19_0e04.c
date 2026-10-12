
int __cdecl16far ai_pick_enemy(void)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 unaff_SS;
  int local_d0 [8];
  int local_c0 [8];
  int local_b0 [16];
  int local_90 [8];
  int local_80 [8];
  int local_70 [8];
  int local_60 [8];
  int local_50 [8];
  int local_40 [8];
  int local_30 [8];
  int iStack_20;
  int local_1e;
  byte abStack_17 [7];
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  local_4 = 2;
  local_6 = 4;
  local_c = -1;
  local_e = FUN_828e_044a(*(undefined2 *)(current_player * 0x14 + 0x193));
  local_10 = *(int *)(current_player * 2 + 0x10e3);
  iVar2 = 8;
  while (iVar5 = iVar2 + -1, iVar8 = city_count, iVar2 != 0) {
    local_60[iVar5] = 0;
    local_c0[iVar5] = 0;
    local_30[iVar5] = 0;
    local_90[iVar5] = 0;
    local_50[iVar5] = 0;
    local_40[iVar5] = 0;
    local_b0[iVar5] = 0;
    iVar2 = dice(1,10,0);
    local_70[iVar5] = iVar2;
    local_80[iVar5] = iVar5;
    iVar2 = iVar5;
  }
  while( true ) {
    iVar5 = iVar8 + -1;
    iVar2 = (int)DAT_451b_018e;
    uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (iVar8 == 0) break;
    iVar8 = iVar5;
    if (*(char *)(iVar5 * 0x41 + 0x1592) != '\x0f') {
      local_30[*(char *)(iVar5 * 0x41 + 0x1592)] = local_30[*(char *)(iVar5 * 0x41 + 0x1592)] + 1;
      if ((*(byte *)(iVar2 + iVar5 + 0x11e) & 1) == 0) {
        local_60[*(char *)(iVar5 * 0x41 + 0x1592)] = local_60[*(char *)(iVar5 * 0x41 + 0x1592)] + 1;
      }
      if ((int)*(char *)(iVar5 * 0x41 + 0x1592) == current_player) {
        if (*(char *)(iVar5 * 0x41 + 0x15ac) != '\x0f') {
          local_40[*(byte *)(iVar5 * 0x41 + 0x15ac)] =
               local_40[*(byte *)(iVar5 * 0x41 + 0x15ac)] + 1;
        }
        FUN_5db9_0656(iVar5,0,abStack_17 + 1);
        iVar2 = 6;
        while (iVar6 = iVar2 + -1, iVar2 != 0) {
          uVar3 = (uint)abStack_17[iVar2];
          iVar2 = iVar6;
          if (((uVar3 != 0xff) && ((int)*(char *)(uVar3 * 0x41 + 0x1592) != current_player)) &&
             (*(byte *)(uVar3 * 0x41 + 0x15ac) == current_player)) {
            local_50[*(char *)(iVar5 * 0x41 + 0x1592)] =
                 local_50[*(char *)(iVar5 * 0x41 + 0x1592)] + 1;
          }
        }
      }
    }
  }
  local_8 = 0;
  iVar8 = *(int *)(iVar2 + 0x24a);
  while (iVar5 = iVar8 + -1, iVar8 != 0) {
    iVar8 = iVar5;
    if (*(int *)(iVar2 + iVar5 * 0x5c + 0x24c) != 0) {
      local_b0[*(int *)(iVar2 + iVar5 * 0x5c + 0x24e)] =
           local_b0[*(int *)(iVar2 + iVar5 * 0x5c + 0x24e)] + 1;
      local_8 = local_8 + 1;
    }
  }
  if ((*(char *)(local_e * 0x41 + 0x1592) != '\x0f') &&
     ((int)*(char *)(local_e * 0x41 + 0x1592) != current_player)) {
    local_c0[*(char *)(local_e * 0x41 + 0x1592)] =
         local_c0[*(char *)(local_e * 0x41 + 0x1592)] + 0x14;
  }
  local_a = 0;
  uVar3 = 8;
  while (uVar7 = uVar3 - 1, uVar3 != 0) {
    uVar3 = uVar7;
    if (local_30[uVar7] != 0) {
      if (current_player != uVar7) {
        local_a = local_a + 1;
      }
      local_1e = FUN_828e_044a(*(undefined2 *)(uVar7 * 0x14 + 0x193));
      if ((int)*(char *)(local_1e * 0x41 + 0x1592) == current_player) {
        local_90[uVar7] = 1;
      }
    }
  }
  iVar2 = 8;
  do {
    iVar8 = iVar2 + -1;
    if (iVar2 == 0) {
      bVar1 = false;
      uVar3 = 8;
      while (uVar7 = uVar3 - 1, uVar3 != 0) {
        uVar3 = uVar7;
        if (((*(int *)(uVar7 * 2 + 0x137) != 0) && (current_player != uVar7)) &&
           ((*(byte *)(current_player * 8 + uVar7 + 0x153b) & 0xc) == 8)) {
          bVar1 = true;
        }
      }
      if ((opt_all_computer != 0) && (*(int *)(current_player * 2 + 0xd0) != 0)) {
        iVar2 = 8;
        while (iVar2 != 0) {
          local_d0[iVar2 + -1] = 0;
          iVar2 = iVar2 + -1;
        }
        iVar2 = 8;
        while (iVar8 = iVar2 + -1, iVar2 != 0) {
          iVar2 = iVar8;
          if ((*(int *)(iVar8 * 2 + 0x137) != 0) && (*(int *)(iVar8 * 2 + 0xd0) != 0)) {
            iVar5 = 8;
            while (iVar6 = iVar5 + -1, iVar5 != 0) {
              iVar5 = iVar6;
              if (((*(int *)(iVar6 * 2 + 0x137) != 0) && (*(int *)(iVar6 * 2 + 0xd0) != 1)) &&
                 ((*(byte *)(iVar8 * 8 + iVar6 + 0x153b) & 3) == 2)) {
                local_d0[iVar8] = 1;
              }
            }
          }
        }
        iVar2 = 8;
        while (iVar8 = iVar2 + -1, iVar2 != 0) {
          iVar2 = iVar8;
          if (((*(int *)(iVar8 * 2 + 0x137) != 0) && (*(int *)(iVar8 * 2 + 0xd0) != 0)) &&
             ((local_d0[current_player] != 0 || (local_d0[iVar8] != 0)))) {
            local_c0[iVar8] = 0;
          }
        }
      }
      iVar2 = 8;
      while( true ) {
        iVar8 = iVar2 + -1;
        uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        if (iVar2 == 0) break;
        iVar2 = (int)DAT_451b_018e + iVar8 * 2;
        local_1e = *(int *)(iVar2 + 0x3dc) + *(int *)(iVar2 + 0x3fc);
        if (opt_quick_start == 0) {
          iStack_20 = 8;
        }
        else {
          iStack_20 = 4;
        }
        iVar2 = FUN_558d_0a6e();
        if (iVar2 != 0) {
          local_c0[iVar8] = 0;
        }
        if ((bVar1) && ((*(byte *)(current_player * 8 + iVar8 + 0x153b) & 0xc) == 0)) {
          local_c0[iVar8] = 0;
        }
        if ((local_8 == 0) && (local_60[iVar8] == 0)) {
          local_c0[iVar8] = 0;
        }
        iVar2 = game_turn();
        if ((iVar2 < iStack_20) && (local_1e == 0)) {
          local_c0[iVar8] = 0;
        }
        iVar2 = iVar8;
        if ((1 < local_a) && (local_30[iVar8] / 4 < local_b0[iVar8])) {
          local_c0[iVar8] = 0;
        }
      }
      local_c0[current_player] = 0;
      iVar2 = 0;
      iVar8 = 8;
      while (iVar5 = iVar8 + -1, iVar8 != 0) {
        iVar8 = iVar5;
        if ((local_30[iVar5] != 0) && (iVar2 < local_c0[iVar5])) {
          iVar2 = local_c0[iVar5];
          local_c = iVar5;
        }
      }
      if ((*(char *)(local_e * 0x41 + 0x1592) != '\x0f') &&
         ((int)*(char *)(local_e * 0x41 + 0x1592) != current_player)) {
        bVar1 = false;
        iVar2 = *(int *)((int)DAT_451b_018e + 0x24a);
        while (iVar8 = iVar2 + -1, iVar2 != 0) {
          iVar2 = iVar8;
          if ((int)*(char *)(local_e * 0x41 + 0x1592) == local_b0[iVar8]) {
            bVar1 = true;
          }
        }
        if (!bVar1) {
          local_c = (int)*(char *)(local_e * 0x41 + 0x1592);
        }
      }
      if ((local_c != -1) && (local_60[local_c] == 0)) {
        local_c = -1;
      }
      return local_c;
    }
    local_c0[iVar8] = local_c0[iVar8] + local_70[iVar8];
    local_c0[iVar8] = local_c0[iVar8] + local_90[iVar8] * 0xf;
    local_c0[iVar8] = local_c0[iVar8] + local_40[iVar8] * 4;
    local_c0[iVar8] = local_c0[iVar8] + local_50[iVar8] * 6;
    uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar2 = (int)DAT_451b_018e;
    iVar5 = iVar2 + iVar8 * 2;
    local_c0[iVar8] = local_c0[iVar8] + *(int *)(iVar5 + 0x3bc) * 4;
    local_c0[iVar8] = local_c0[iVar8] + *(int *)(iVar5 + 0x3cc);
    local_c0[iVar8] = local_c0[iVar8] + *(int *)(iVar5 + 0x3dc);
    local_c0[iVar8] = local_c0[iVar8] + *(int *)(iVar5 + 0x3ec) * 2;
    local_c0[iVar8] = local_c0[iVar8] + *(int *)(iVar5 + 0x3fc) * 2;
    local_c0[iVar8] = local_c0[iVar8] + *(int *)(iVar5 + 0x40c) * 2;
    if (*(int *)(current_player * 2 + 0xd0) == 0) {
      piVar4 = local_c0 + iVar8;
      iVar2 = *(int *)(iVar2 + 0x18);
LAB_5f19_12f0:
      *piVar4 = *piVar4 + iVar2;
    }
    else {
      if (*(int *)(current_player * 2 + 0xc0) == 2) {
        piVar4 = local_c0 + iVar8;
        iVar2 = *(int *)(iVar2 + 0x1e);
        goto LAB_5f19_12f0;
      }
      if (*(int *)(current_player * 2 + 0xc0) == 1) {
        piVar4 = local_c0 + iVar8;
        iVar2 = *(int *)(iVar2 + 0x1a);
        goto LAB_5f19_12f0;
      }
      if (*(int *)(current_player * 2 + 0xc0) == 0) {
        piVar4 = local_c0 + iVar8;
        iVar2 = *(int *)(iVar2 + 0x1c);
        goto LAB_5f19_12f0;
      }
    }
    uVar3 = local_10 - *(int *)(iVar8 * 2 + 0x10e3);
    uVar7 = (int)uVar3 >> 0xf;
    local_c0[iVar8] = local_c0[iVar8] + (int)((uVar3 ^ uVar7) - uVar7) / 8;
    uVar3 = local_30[current_player] - local_30[iVar8] >> 0xf;
    local_c0[iVar8] =
         local_c0[iVar8] + (int)((local_30[current_player] - local_30[iVar8] ^ uVar3) - uVar3) / 4;
    iVar2 = iVar8;
  } while( true );
}


