
undefined2 __cdecl16far FUN_57ea_10a8(int param_1,int param_2,int param_3)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  int local_18 [8];
  int local_8;
  int local_6;
  int local_4;
  
  local_6 = -1;
  iVar3 = city_count;
  do {
    do {
      iVar4 = iVar3 + -1;
      iVar2 = local_6;
      if (iVar3 == 0) goto LAB_57ea_119f;
      iVar3 = iVar4;
    } while ((((*(char *)(*(byte *)(*(int *)(iVar4 * 0x41 + 0x157f) * 0xe0 +
                                   *(int *)(iVar4 * 0x41 + 0x157d) * 2) + 0x710) != '\n') ||
              (*(char *)(iVar4 * 0x41 + 0x1592) == current_player)) ||
             (*(int *)((int)DAT_451b_018e + 0x46) == iVar4)) ||
            ((iVar2 = map_distance(*(int *)(iVar4 * 0x41 + 0x157d),*(int *)(iVar4 * 0x41 + 0x157f),
                                   param_2,param_3), 2 < iVar2 ||
             (local_8 = FUN_558d_0851(), local_8 == 0))));
    if ((*(byte *)((int)DAT_451b_018e + iVar4 + 0x11e) & 1) != 0) {
      ai_city_seen_check();
    }
    iVar2 = iVar4;
  } while ((*(byte *)((int)DAT_451b_018e + iVar4 + 0x11e) & 1) != 0);
LAB_57ea_119f:
  local_6 = iVar2;
  if (local_6 == -1) {
    return 0;
  }
  iVar3 = 8;
  while (iVar3 != 0) {
    local_18[iVar3 + -1] = -1;
    iVar3 = iVar3 + -1;
  }
  local_18[0] = param_1;
  FUN_623c_0ae7(local_18);
  local_4 = FUN_623c_15c5();
  if ((*(char *)(param_1 * 0x16 + -0x777c) != '\x1c') &&
     (*(char *)(param_1 * 0x16 + -0x7778) < '\x04')) {
    if (*(int *)((int)DAT_451b_018e + 6) < city_count / 10) {
      iVar3 = 0x32;
      goto LAB_57ea_1280;
    }
    iVar3 = game_turn();
    if (iVar3 / 2 < 0xb) {
      iVar3 = game_turn();
      iVar3 = iVar3 / 2;
    }
    else {
      iVar3 = 10;
    }
    if (*(int *)((int)DAT_451b_018e + 2) <= iVar3) {
      iVar3 = 0x28;
      goto LAB_57ea_1280;
    }
  }
  iVar3 = 0x4b;
LAB_57ea_1280:
  if (local_4 < iVar3) {
    return 0;
  }
  iVar3 = 8;
  while (iVar3 != 0) {
    local_18[iVar3 + -1] = -1;
    iVar3 = iVar3 + -1;
  }
  local_18[0] = param_1;
  puVar1 = (uint *)(param_1 * 0x16 + -0x7774);
  *puVar1 = *puVar1 & 0xffdf;
  FUN_623c_0ae7(local_18);
  uVar5 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
  move_stack_to(0x623c,*(undefined2 *)((int)DAT_451b_1f02 + 0x12),
                *(undefined2 *)((int)DAT_451b_1f02 + 0x14),0);
  return 1;
}


