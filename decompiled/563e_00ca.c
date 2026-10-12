
int __cdecl16far ai_run_assault_group(int group)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  
  iVar2 = FUN_828e_044a(*(undefined2 *)(current_player * 0x14 + 0x193),
                        *(undefined2 *)(current_player * 0x14 + 0x195));
  if ((*(char *)(iVar2 * 0x41 + 0x1592) != '\x0f') &&
     (*(char *)(iVar2 * 0x41 + 0x1592) != current_player)) {
    bVar1 = false;
    uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar4 = (int)DAT_451b_018e;
    iVar5 = *(int *)(iVar4 + 0x24a);
    while (iVar3 = iVar5 + -1, iVar5 != 0) {
      iVar5 = iVar3;
      if ((*(int *)(iVar4 + iVar3 * 0x5c + 0x24c) != 0) &&
         (((int)*(char *)(iVar2 * 0x41 + 0x1592) == *(int *)(iVar4 + iVar3 * 0x5c + 0x24e) ||
          (*(int *)(iVar4 + iVar3 * 0x5c + 0x24c) == 0)))) {
        bVar1 = true;
      }
    }
    if (!bVar1) {
      ai_abandon_assaults();
      return 0;
    }
  }
  iVar2 = ai_assault_revalidate(group);
  if (iVar2 == 0) {
    FUN_563e_066d(group,0);
  }
  else {
    FUN_563e_1579(group);
    bVar1 = false;
    uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar5 = (int)DAT_451b_018e;
    iVar2 = *(int *)(iVar5 + 0x24a);
    while (iVar4 = iVar2 + -1, iVar2 != 0) {
      iVar2 = iVar4;
      if (((*(int *)(iVar5 + iVar4 * 0x5c + 0x24c) != 0) && (iVar4 != group)) &&
         (*(int *)(iVar5 + iVar4 * 0x5c + 0x250) == *(int *)(iVar5 + group * 0x5c + 0x250))) {
        bVar1 = true;
      }
    }
    if (((!bVar1) &&
        (*(char *)(*(int *)(iVar5 + group * 0x5c + 0x250) * 0x41 + 0x1592) == current_player)) ||
       (iVar2 = FUN_563e_02ed(group), iVar2 != 0)) {
      ai_assault_gather(group);
      uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      if (*(int *)((int)DAT_451b_018e + group * 0x5c + 0x24c) != 0) {
        ai_city_garrison_check(*(undefined2 *)((int)DAT_451b_018e + group * 0x5c + 0x250),1);
        iVar2 = ai_assault_attack(group);
        if (iVar2 != 0) {
          uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
          if (-1 < *(int *)((int)DAT_451b_018e + group * 0x5c + 0x250)) {
            ai_city_garrison_check(*(undefined2 *)((int)DAT_451b_018e + group * 0x5c + 0x250),1);
          }
        }
        if (*(int *)((int)DAT_451b_018e + group * 0x5c + 0x24c) != 0) {
          FUN_563e_16fd(group);
          return 1;
        }
      }
    }
  }
  return 0;
}


