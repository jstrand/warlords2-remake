
undefined2 __cdecl16far FUN_563e_0f1b(int param_1,int *param_2,int param_3)

{
  char cVar1;
  int iVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  int local_c;
  int local_a;
  
  iVar5 = *param_2;
  if ((iVar5 != -1) &&
     (*(char *)(*(byte *)(*(int *)(param_3 * 0x41 + 0x157f) * 0xe0 +
                         *(int *)(param_3 * 0x41 + 0x157d) * 2) + 0x710) != '\n')) {
    local_a = 1000;
    local_c = -1;
    for (iVar6 = 0; iVar6 < 6; iVar6 = iVar6 + 1) {
      iVar2 = *(int *)((int)DAT_451b_018e + param_1 * 0x5c + iVar6 * 2 + 0x25a);
      if (((iVar2 != -1) &&
          (iVar4 = map_distance(*(int *)(iVar5 * 0x16 + -0x7780),*(int *)(iVar5 * 0x16 + -0x777e),
                                *(int *)(iVar2 * 0x41 + 0x157d),*(int *)(iVar2 * 0x41 + 0x157f)),
          iVar4 < local_a)) &&
         (*(char *)(*(byte *)(*(int *)(iVar2 * 0x41 + 0x157f) * 0xe0 +
                             *(int *)(iVar2 * 0x41 + 0x157d) * 2) + 0x710) == '\n')) {
        local_c = iVar2;
        local_a = iVar4;
      }
    }
    if (local_c != -1) {
      param_3 = local_c;
    }
  }
  while( true ) {
    iVar5 = 0;
    for (iVar6 = 0; iVar6 < 6; iVar6 = iVar6 + 1) {
      if (*(int *)((int)DAT_451b_018e + param_1 * 0x5c + iVar6 * 2 + 0x25a) != -1) {
        iVar5 = iVar5 + 1;
      }
    }
    for (iVar6 = 0; iVar6 < 8; iVar6 = iVar6 + 1) {
      uVar7 = (undefined2)((ulong)param_2 >> 0x10);
      iVar2 = ((int *)param_2)[iVar6];
      if ((iVar2 != -1) && (*(char *)(iVar2 * 0x16 + -0x777b) != current_player)) {
        ((int *)param_2)[iVar6] = -1;
      }
    }
    cVar1 = *(char *)(param_3 * 0x41 + 0x1592);
    FUN_623c_0ae7(param_2,1,param_3,0);
    uVar7 = 0;
    iVar6 = move_stack_to();
    if (iVar6 == 1) {
      FUN_5ca7_0000(1);
      FUN_563e_1251(param_1,uVar7);
      return 3;
    }
    FUN_563e_1251(param_1,uVar7);
    if (*(char *)(param_3 * 0x41 + 0x1592) != current_player) {
      return 3;
    }
    if (current_player == cVar1) break;
    if ((*(int *)(param_3 * 0x41 + 0x157d) == *(int *)(current_player * 0x14 + 0x193)) &&
       (*(int *)(param_3 * 0x41 + 0x157f) == *(int *)(current_player * 0x14 + 0x195))) {
      bVar3 = true;
    }
    else {
      bVar3 = false;
    }
    iVar6 = (int)DAT_451b_018e;
    uVar7 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (((bVar3) || (iVar5 < 2)) || ((*(uint *)(iVar6 + param_1 * 0x5c + 0x2a6) & 1) == 0)) {
      if ((bVar3) || ((*(uint *)(iVar6 + param_1 * 0x5c + 0x2a6) & 2) == 0)) {
        if ((!bVar3) && ((*(uint *)(iVar6 + param_1 * 0x5c + 0x2a6) & 4) != 0)) {
          auto_str_is_being_pillaged(param_3);
        }
      }
      else {
        auto_str_is_being_sacked(param_3);
      }
      FUN_563e_134c(param_1,param_3);
      return 1;
    }
    param_3 = auto_str_is_being_razed(param_3,0);
    if (param_3 == -1) {
      return 1;
    }
  }
  return 3;
}


