
undefined2 __cdecl16far quest_choose_reward(int *param_1)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  uint uVar4;
  char *pcVar5;
  char *pcVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int local_a;
  char *local_8;
  int local_6;
  
  if (((int)((undefined2 *)&side_city_count)[current_player] < 10) &&
     (iVar1 = game_turn(), 0xf < iVar1)) {
    iVar1 = 5;
LAB_4976_1930:
    iVar1 = dice(1,3,iVar1);
    *param_1 = iVar1;
    uVar2 = 3;
  }
  else {
    if (99 < *(int *)(current_player * 0x14 + 0x185)) {
      iVar1 = 0;
      for (iVar8 = 0; iVar8 < 0x16; iVar8 = iVar8 + 1) {
        if (*(char *)(iVar8 * 0x1d + 0xcff) == '\0') {
          __F_SPUSH();
          iVar9 = item_reserved();
          if ((iVar9 != 0) && ((7 < iVar8 || (current_player == iVar8)))) {
            iVar1 = iVar1 + 1;
          }
        }
      }
      local_6 = 0;
      for (iVar8 = 0; iVar8 < site_count; iVar8 = iVar8 + 1) {
        if (((*(int *)(iVar8 * 0x1f + 0x82c) != 0) &&
            ((*(uint *)(iVar8 * 0x1f + 0x82e) & 1 << ((byte)current_player & 0x1f)) == 0)) &&
           (*(char *)(iVar8 * 0x1f + 0x829) != '\0')) {
          local_6 = local_6 + 1;
        }
      }
      if ((0 < iVar1) && (iVar8 = dice(1,3,0), iVar8 == 1)) {
        iVar1 = dice(1,iVar1,0);
        iVar8 = 0;
        for (iVar9 = 0; iVar9 < 0x16; iVar9 = iVar9 + 1) {
          if (*(char *)(iVar9 * 0x1d + 0xcff) == '\0') {
            __F_SPUSH();
            iVar3 = item_reserved();
            if (((iVar3 != 0) && ((7 < iVar9 || (current_player == iVar9)))) &&
               (iVar8 = iVar8 + 1, iVar8 == iVar1)) break;
          }
        }
        *param_1 = iVar9;
        return 0;
      }
      if (((iVar1 < 1) && (0 < local_6)) && (iVar1 = dice(1,3,0), iVar1 < 3)) {
        local_8 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
        local_a = 0;
        for (iVar1 = 0; iVar1 < site_count; iVar1 = iVar1 + 1) {
          if (((*(int *)(iVar1 * 0x1f + 0x82c) != 0) &&
              ((*(uint *)(iVar1 * 0x1f + 0x82e) & 1 << ((byte)current_player & 0x1f)) == 0)) &&
             (*(char *)(iVar1 * 0x1f + 0x829) != '\0')) {
            uVar4 = *(int *)(iVar1 * 0x1f + 0x811) -
                    *(int *)(*(int *)(current_player * 0xc + 0x1107) * 0x16 + -0x7780);
            uVar7 = (int)uVar4 >> 0xf;
            pcVar5 = (char *)((uVar4 ^ uVar7) - uVar7);
            uVar4 = *(int *)(iVar1 * 0x1f + 0x813) -
                    *(int *)(*(int *)(current_player * 0xc + 0x1107) * 0x16 + -0x777e);
            uVar7 = (int)uVar4 >> 0xf;
            pcVar6 = (char *)((uVar4 ^ uVar7) - uVar7);
            if ((int)pcVar6 < (int)pcVar5) {
              pcVar6 = pcVar5;
            }
            if ((int)pcVar6 < (int)local_8) {
              local_a = iVar1;
              local_8 = pcVar6;
            }
          }
        }
        *param_1 = local_a;
        return 1;
      }
      iVar1 = dice(1,2,0);
      if (iVar1 == 1) {
        iVar1 = 2;
        goto LAB_4976_1930;
      }
    }
    iVar1 = dice(2,1000,1000);
    *param_1 = iVar1;
    uVar2 = 2;
  }
  return uVar2;
}


