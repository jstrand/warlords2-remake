
void __cdecl16far ai_sage_visit(int site)

{
  int *piVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined2 unaff_SS;
  int local_20;
  int local_1e;
  byte abStack_13 [7];
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_4 = dice(3,500,500);
  local_6 = -1;
  local_8 = -1;
  local_a = -1;
  local_c = 0;
  iVar3 = city_count;
  while (iVar8 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar8;
    if ((*(char *)(iVar8 * 0x41 + 0x1592) == '\x0f') &&
       ((*(byte *)((int)DAT_451b_018e + iVar8 + 0x11e) & 1) != 0)) {
      FUN_5db9_0656(iVar8,0,abStack_13 + 1);
      bVar2 = false;
      iVar5 = 6;
      while (iVar4 = iVar5, iVar5 = iVar4 + -1, iVar4 != 0) {
        if ((abStack_13[iVar4] != 0xff) &&
           (*(char *)((uint)abStack_13[iVar4] * 0x41 + 0x1592) == current_player)) {
          bVar2 = true;
        }
      }
      if (bVar2) {
        iVar5 = *(int *)(iVar8 * 0x41 + 0x157d);
        iVar4 = *(int *)(iVar8 * 0x41 + 0x157f);
        local_1e = 0;
        local_20 = 1000;
        iVar6 = city_count;
        while (iVar7 = iVar6 + -1, iVar6 != 0) {
          iVar6 = iVar7;
          if (*(char *)(iVar7 * 0x41 + 0x1592) == '\x0f') {
            if (((*(byte *)((int)DAT_451b_018e + iVar7 + 0x11e) & 1) != 0) &&
               (iVar7 = map_distance(iVar5,iVar4,*(int *)(iVar7 * 0x41 + 0x157d),
                                     *(int *)(iVar7 * 0x41 + 0x157f)), iVar7 < 0x14)) {
              local_1e = local_1e + 1;
            }
          }
          else if ((*(char *)(iVar7 * 0x41 + 0x1592) == current_player) &&
                  (iVar7 = map_distance(iVar5,iVar4,*(int *)(iVar7 * 0x41 + 0x157d),
                                        *(int *)(iVar7 * 0x41 + 0x157f)), iVar7 < local_20)) {
            local_20 = iVar7;
          }
        }
        if ((3 < local_1e) &&
           ((local_a < local_1e || ((local_1e == local_a && (local_c < local_20)))))) {
          local_c = local_20;
          local_a = local_1e;
          local_6 = *(int *)(iVar8 * 0x41 + 0x157d);
          local_8 = *(int *)(iVar8 * 0x41 + 0x157f);
        }
      }
    }
  }
  if ((opt_hidden_map == 0) || (local_6 == -1)) {
    piVar1 = (int *)(current_player * 0x14 + 0x185);
    *piVar1 = *piVar1 + local_4;
  }
  else {
    iVar3 = dice(1,0xb,-6);
    iVar8 = dice(1,0xb,-6);
    if (SCARRY2(local_6,iVar3) == local_6 + iVar3 < 0) {
      local_6 = local_6 + iVar3;
    }
    else {
      local_6 = 0;
    }
    if (SCARRY2(local_8,iVar8) == local_8 + iVar8 < 0) {
      local_8 = local_8 + iVar8;
    }
    else {
      local_8 = 0;
    }
    if (0x6e < local_6) {
      local_6 = 0x6f;
    }
    if (0x9a < local_8) {
      local_8 = 0x9b;
    }
    reveal_map_area(local_6,local_8);
    if (((DAT_3c04_0159 != 0) && (DAT_4125_3254 == 0)) || (*(int *)(current_player * 2 + 0xd0) == 0)
       ) {
      FUN_834b_2200();
      FUN_22bf_07a5();
      FUN_834b_0000(0x10001,0);
      FUN_22bf_0757();
    }
  }
  return;
}


