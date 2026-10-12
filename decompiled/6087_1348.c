
void __cdecl16far auto_str_enemy_city_combat_dist(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  int local_22 [8];
  int local_12;
  int local_10;
  uint local_e;
  int local_c;
  uint local_a;
  int local_8;
  int local_6;
  int local_4;
  
  uVar6 = (undefined2)((ulong)param_2 >> 0x10);
  iVar3 = (int)param_2;
  local_8 = *(int *)(iVar3 + param_1 * 2);
  local_e = 0xffff;
  local_10 = 0x14;
  local_12 = 0;
  *(undefined2 *)(iVar3 + 0x3a) = 0xffff;
  *(undefined2 *)(iVar3 + 0x38) = 0xffff;
  if ((*(byte *)(local_8 * 0x16 + -0x7772) & 0xf) == 1) {
    local_a = (uint)(*(byte *)(local_8 * 0x16 + -0x7771) >> 1);
  }
  else {
    local_a = 0xffff;
  }
  local_6 = ai_collect_stack(*(int *)(local_8 * 0x16 + -0x7780),*(int *)(local_8 * 0x16 + -0x777e),
                             (int *)CONCAT22(unaff_SS,local_22),
                             (*(byte *)(local_8 * 0x16 + -0x7772) & 0x70) >> 4,
                             *(byte *)(local_8 * 0x16 + -0x7772) & 0xf,0xc);
  if (local_6 != 0) {
    if (local_6 < 3) {
      local_4 = 0x5f;
      uVar5 = city_count;
    }
    else {
      local_4 = 0x4b;
      uVar5 = city_count;
    }
    while (uVar4 = uVar5, uVar5 = uVar4 - 1, uVar4 != 0) {
      if (((((*(char *)(*(byte *)(*(int *)(uVar5 * 0x41 + 0x157f) * 0xe0 +
                                 *(int *)(uVar5 * 0x41 + 0x157d) * 2) + 0x710) == '\n') &&
            ((*(byte *)((int)DAT_451b_018e + uVar5 + 0x11e) & 1) == 0)) &&
           (*(char *)(uVar5 * 0x41 + 0x1592) != '\x0f')) &&
          ((*(byte *)(current_player * 8 + (int)*(char *)(uVar5 * 0x41 + 0x1592) + 0x153b) & 3) == 2
          )) && ((local_c = map_distance(*(int *)(uVar5 * 0x41 + 0x157d),
                                         *(int *)(uVar5 * 0x41 + 0x157f),
                                         *(int *)(local_8 * 0x16 + -0x7780),
                                         *(int *)(local_8 * 0x16 + -0x777e)), local_c <= local_10 ||
                 (local_a == uVar5)))) {
        iVar1 = FUN_623c_15c5(*(undefined2 *)(uVar5 * 0x41 + 0x157d),
                              *(undefined2 *)(uVar5 * 0x41 + 0x157f));
        if (local_a == uVar5) {
          iVar1 = iVar1 + 0x14;
        }
        iVar2 = map_distance(*(int *)(uVar5 * 0x41 + 0x157d),*(int *)(uVar5 * 0x41 + 0x157f),
                             *(int *)(local_8 * 0x16 + -0x7780),*(int *)(local_8 * 0x16 + -0x777e));
        sprintf(&DAT_451b_1d28,s_Enemy_city__d__combat__d_dist__d_4125_09ef,uVar4,iVar1,iVar2);
        FUN_623c_170c();
        if ((local_4 <= iVar1) &&
           ((local_12 < iVar1 || ((iVar1 == local_12 && (local_c < local_10)))))) {
          local_10 = local_c;
          local_12 = iVar1;
          local_e = uVar5;
        }
      }
    }
    *(uint *)(iVar3 + 0x38) = local_e;
    *(int *)(iVar3 + 0x3a) = local_10;
  }
  return;
}


