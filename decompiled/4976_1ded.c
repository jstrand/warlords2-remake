
int __cdecl16far quest_check(int param_1,undefined2 param_2,int param_3,undefined4 param_4)

{
  int *piVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_SS;
  char *pcVar6;
  char *pcVar7;
  int local_14;
  int iStack_12;
  int iStack_10;
  int iStack_e;
  undefined2 local_8;
  int local_6;
  int local_4;
  
  iVar5 = 0;
  local_4 = 0;
  __F_SCOPY(current_player * 0xc + 0x1103,0x3c04,&local_14,unaff_SS);
  if (local_14 == 0) {
    return 0;
  }
  local_6 = *(int *)(current_player * 0xc + 0x1107);
  if (*(char *)(local_6 * 0x16 + -0x777b) != current_player) {
    iVar5 = is_computer_turn();
    if (iVar5 != 0) goto LAB_4976_1e8c;
    pcVar6 = get_string(0x20,1);
    iVar5 = 0x20;
LAB_4976_1e7a:
    pcVar7 = get_string(iVar5,0);
    FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
    goto LAB_4976_1e8c;
  }
  if (5 < param_1 + 1U) goto LAB_4976_26e5;
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)
         *(undefined2 *)
          ((char *)s__h__Do_not_use_expanded_or_exten_4125_271d + 0x28 + (param_1 + 1U) * 2)) {
  case 0x4b61b:
    if ((iStack_12 != 4) ||
       (*(char *)(*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(iStack_e * 0x41 + 0x157d) * 2) + 0x710) == '\n')) {
      if ((iStack_12 == 5) &&
         (*(char *)(*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0xe0 +
                             *(int *)(iStack_e * 0x41 + 0x157d) * 2) + 0x710) != '\n')) {
        iVar5 = is_computer_turn();
        if (iVar5 == 0) {
          pcVar6 = get_string(0x21,1);
          pcVar7 = get_string(0x21,0);
          FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
        }
      }
      else if (((iStack_12 == 5) || (iStack_12 == 4)) &&
              (*(char *)(iStack_e * 0x41 + 0x1592) == current_player)) {
        iVar5 = is_computer_turn();
        if (iVar5 == 0) {
          pcVar6 = get_string(0x2a,1);
          pcVar7 = get_string(0x2a,0);
          FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
        }
      }
      else if ((iStack_12 == 0) && (*(char *)(iStack_e * 0x16 + -0x777c) != '\x1c')) {
        iVar5 = is_computer_turn();
        if (iVar5 == 0) {
          pcVar6 = get_string(0x29,1);
          pcVar7 = get_string(0x29,0);
          FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
        }
      }
      else if ((iStack_12 == 3) && (*(int *)(iStack_e * 2 + 0x137) == 0)) {
        iVar5 = is_computer_turn();
        if (iVar5 == 0) {
          pcVar6 = get_string(0x27,1);
          pcVar7 = get_string(0x27,0);
          FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
        }
      }
      else {
        if ((iStack_12 != 1) || (*(char *)(iStack_e * 0x1d + 0xcff) != '\0')) goto LAB_4976_26e5;
        iVar5 = is_computer_turn();
        if (iVar5 == 0) {
          pcVar6 = get_string(0x28,1);
          pcVar7 = get_string(0x28,0);
          FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
        }
      }
      goto LAB_4976_1e8c;
    }
    iVar5 = is_computer_turn();
    if (iVar5 != 0) goto LAB_4976_1e8c;
    pcVar6 = get_string(0x21,1);
    iVar5 = 0x21;
    goto LAB_4976_1e7a;
  case 0x4b85e:
    for (iVar3 = 0; iVar3 < combat_atk_count; iVar3 = iVar3 + 1) {
      if ((((undefined2 *)&DAT_451b_0368)[iVar3 * 2] == 0x2e1d) &&
         (iStack_10 * 0x16 + -0x7780 == ((undefined2 *)&combat_atk_armies)[iVar3 * 2])) {
        local_4 = 1;
        break;
      }
    }
    if (local_4 == 0) goto LAB_4976_26e5;
    if ((iStack_12 != 3) || (DAT_451b_029b != iStack_e)) {
      if (iStack_12 == 2) {
        for (iVar3 = 0; iVar3 < combat_def_count; iVar3 = iVar3 + 1) {
          if ((((undefined1 *)&combat_def_alive)[iVar3] == '\0') &&
             ((char)((undefined1 *)&combat_def_type)[iVar3] == iStack_e)) goto LAB_4976_21b0;
        }
      }
      else if (iStack_12 == 0) {
        for (iVar3 = 0; iVar3 < combat_def_count; iVar3 = iVar3 + 1) {
          if (((((undefined1 *)&combat_def_alive)[iVar3] == '\0') &&
              (*(int *)((undefined *)&DAT_451b_0388 + iVar3 * 4) == 0x2e1d)) &&
             (iStack_e * 0x16 + -0x7780 == ((undefined2 *)&combat_def_armies)[iVar3 * 2]))
          goto LAB_4976_21b0;
        }
      }
      goto LAB_4976_26e5;
    }
    iVar3 = 0;
    for (iVar4 = 0; iVar4 < combat_def_count; iVar4 = iVar4 + 1) {
      if (((undefined1 *)&combat_def_alive)[iVar4] == '\0') {
        iVar3 = iVar3 + 1;
      }
    }
    piVar1 = (int *)(current_player * 0xc + 0x110d);
    *piVar1 = *piVar1 + iVar3;
    if (*(int *)(current_player * 0xc + 0x110d) < *(int *)(current_player * 0xc + 0x110b))
    goto LAB_4976_26e5;
    break;
  case 0x4b988:
    for (iVar3 = 0; iVar3 < combat_atk_count; iVar3 = iVar3 + 1) {
      if ((((undefined2 *)&DAT_451b_0368)[iVar3 * 2] == 0x2e1d) &&
         (iStack_10 * 0x16 + -0x7780 == ((undefined2 *)&combat_atk_armies)[iVar3 * 2])) {
        local_4 = 1;
        break;
      }
    }
    if (((iStack_12 == 4) && (*(char *)(iStack_e * 0x41 + 0x1592) == current_player)) &&
       (*(char *)(*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(iStack_e * 0x41 + 0x157d) * 2) + 0x710) == '\n')) {
      if (local_4 == 0) {
        *(undefined2 *)(current_player * 0xc + 0x1103) = 0;
        iVar5 = is_computer_turn();
        if (iVar5 != 0) {
          return 0;
        }
        pcVar6 = get_string(0x22,1);
        iVar5 = 0x22;
LAB_4976_2605:
        pcVar7 = get_string(iVar5,0);
        FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
        return 0;
      }
      iVar5 = 1;
    }
    if (((iStack_12 != 5) || (*(char *)(iStack_e * 0x41 + 0x1592) != current_player)) ||
       (*(char *)(*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(iStack_e * 0x41 + 0x157d) * 2) + 0x710) != '\n'))
    goto LAB_4976_26e5;
    iVar5 = is_computer_turn();
    if (iVar5 == 0) {
      pcVar6 = get_string(0x23,1);
      pcVar7 = get_string(0x23,0);
      FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
    }
    goto LAB_4976_1e8c;
  case 0x4bb18:
    for (iVar3 = 0; iVar3 < combat_atk_count; iVar3 = iVar3 + 1) {
      if ((((undefined2 *)&DAT_451b_0368)[iVar3 * 2] == 0x2e1d) &&
         (iStack_10 * 0x16 + -0x7780 == ((undefined2 *)&combat_atk_armies)[iVar3 * 2])) {
        local_4 = 1;
        break;
      }
    }
    if ((((iStack_12 == 5) || (iStack_12 == 4)) &&
        (*(char *)(iStack_e * 0x41 + 0x1592) == current_player)) &&
       (*(char *)(*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(iStack_e * 0x41 + 0x157d) * 2) + 0x710) == '\n')) {
      iVar5 = is_computer_turn();
      if (iVar5 == 0) {
        pcVar6 = get_string(0x24,1);
        pcVar7 = get_string(0x24,0);
        FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
      }
      goto LAB_4976_1e8c;
    }
    if (((local_4 == 0) || (iStack_12 != 6)) ||
       (piVar1 = (int *)(current_player * 0xc + 0x110d), *piVar1 = *piVar1 + param_3,
       *(int *)(current_player * 0xc + 0x110d) < *(int *)(current_player * 0xc + 0x110b)))
    goto LAB_4976_26e5;
    break;
  case 0x4bc57:
    for (iVar3 = 0; iVar3 < combat_atk_count; iVar3 = iVar3 + 1) {
      if ((((undefined2 *)&DAT_451b_0368)[iVar3 * 2] == 0x2e1d) &&
         (iStack_10 * 0x16 + -0x7780 == ((undefined2 *)&combat_atk_armies)[iVar3 * 2])) {
        local_4 = 1;
        break;
      }
    }
    if (((iStack_12 == 5) && (*(char *)(iStack_e * 0x41 + 0x1592) == '\x0f')) &&
       ((*(char *)(*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0xe0 +
                            *(int *)(iStack_e * 0x41 + 0x157d) * 2) + 0x710) != '\n' &&
        (iVar3 = FUN_828e_044a(*(undefined2 *)(iStack_10 * 0x16 + -0x7780)), iVar3 == iStack_e)))) {
      if (local_4 == 0) {
        *(undefined2 *)(current_player * 0xc + 0x1103) = 0;
        iVar5 = is_computer_turn();
        if (iVar5 != 0) {
          return 0;
        }
        pcVar6 = get_string(0x25,1);
        iVar5 = 0x25;
        goto LAB_4976_2605;
      }
      iVar5 = 1;
    }
    if ((iStack_12 != 4) ||
       (*(char *)(*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(iStack_e * 0x41 + 0x157d) * 2) + 0x710) == '\n'))
    goto LAB_4976_26e5;
    iVar5 = is_computer_turn();
    if (iVar5 == 0) {
      pcVar6 = get_string(0x26,1);
      pcVar7 = get_string(0x26,0);
      FUN_8065_1160((char *)pcVar7,(int)((ulong)pcVar7 >> 0x10),pcVar6);
    }
LAB_4976_1e8c:
    *(undefined2 *)(current_player * 0xc + 0x1103) = 0;
    return 0;
  case 0x4be03:
    if (((iStack_12 != 1) || (*(char *)(iStack_e * 0x1d + 0xcff) != '\x03')) ||
       (*(int *)(iStack_e * 0x1d + 0xd00) != iStack_10)) goto LAB_4976_26e5;
    *(undefined1 *)(iStack_e * 0x1d + 0xcff) = 0;
  }
LAB_4976_21b0:
  iVar5 = 1;
LAB_4976_26e5:
  if (iVar5 != 0) {
    *(undefined2 *)(current_player * 0xc + 0x1103) = 0;
    DAT_451b_0004 = param_4;
    uVar2 = quest_choose_reward(&local_8);
    quest_give_reward(uVar2);
    iVar3 = is_computer_turn();
    if (iVar3 == 0) {
      music_cue(3);
      FUN_4976_1520(uVar2,local_8);
    }
  }
  return iVar5;
}


