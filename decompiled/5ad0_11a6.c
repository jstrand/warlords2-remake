
int __cdecl16far ai_send_hero_party(int *stack)

{
  byte *pbVar1;
  undefined1 *puVar2;
  int y1;
  bool bVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  undefined2 uVar13;
  int local_c;
  int local_8;
  int local_6;
  uint local_4;
  
  local_c = 0x28;
  local_8 = -1;
  local_6 = -1;
  local_4 = 0xffff;
  iVar5 = -1;
  iVar12 = -1;
  iVar8 = 0;
  while( true ) {
    piVar7 = (int *)stack;
    uVar13 = (undefined2)((ulong)stack >> 0x10);
    if (7 < iVar8) break;
    if ((iVar12 == -1) && (*(char *)(piVar7[iVar8] * 0x16 + -0x777c) == '\x1c')) {
      iVar12 = piVar7[iVar8];
    }
    if ((iVar5 == -1) &&
       (((undefined1 *)&army_move_flags)[*(char *)(piVar7[iVar8] * 0x16 + -0x777c) * 6] != '\0')) {
      iVar5 = piVar7[iVar8];
    }
    iVar8 = iVar8 + 1;
  }
  if (iVar12 != -1) {
    if (iVar5 == -1) {
      return 1;
    }
    iVar8 = *(int *)(iVar12 * 0x16 + -0x7780);
    y1 = *(int *)(iVar12 * 0x16 + -0x777e);
    for (iVar9 = 0; iVar9 < 8; iVar9 = iVar9 + 1) {
      piVar7[iVar9] = -1;
    }
    *stack = iVar12;
    piVar7[1] = iVar5;
    pbVar1 = (byte *)(iVar5 * 0x16 + -0x7772);
    *pbVar1 = *pbVar1 & 0xf0;
    pbVar1 = (byte *)(iVar12 * 0x16 + -0x7772);
    *pbVar1 = *pbVar1 & 0xf0;
    puVar2 = (undefined1 *)(iVar12 * 0x16 + -0x7772);
    *puVar2 = *puVar2;
    pbVar1 = (byte *)(iVar5 * 0x16 + -0x7771);
    *pbVar1 = *pbVar1 & 1;
    pbVar1 = (byte *)(iVar12 * 0x16 + -0x7771);
    *pbVar1 = *pbVar1 & 1;
    puVar2 = (undefined1 *)(iVar12 * 0x16 + -0x7771);
    *puVar2 = *puVar2;
    uVar10 = site_count;
LAB_5ad0_1463:
    uVar11 = uVar10 - 1;
    iVar5 = city_count;
    if (uVar10 != 0) {
      bVar3 = false;
      iVar5 = *(int *)(uVar11 * 0x1f + 0x811);
      iVar9 = *(int *)(uVar11 * 0x1f + 0x813);
      iVar4 = army_count;
      do {
        iVar6 = iVar4 + -1;
        if (iVar4 == 0) goto LAB_5ad0_1395;
        iVar4 = iVar6;
      } while ((((*(char *)(iVar6 * 0x16 + -0x777b) != (char)current_player) || (iVar12 == iVar6))
               || (*(char *)(iVar6 * 0x16 + -0x777c) != '\x1c')) ||
              (((*(byte *)(iVar6 * 0x16 + -0x7772) & 0xf) != 3 ||
               (*(byte *)(iVar6 * 0x16 + -0x7771) >> 1 != uVar11))));
      bVar3 = true;
LAB_5ad0_1395:
      uVar10 = uVar11;
      if (((!bVar3) && (iVar4 = FUN_623c_16ae(iVar5,iVar9), iVar4 != 0)) &&
         ((*(char *)(*(byte *)(iVar9 * 0xe0 + iVar5 * 2) + 0x710) == '\v' &&
          ((*(char *)(uVar11 * 0x1f + 0x829) != '\x01' &&
           ((*(byte *)(iVar9 * 0xe0 + iVar5 * 2 + 1) & 0x40) == 0)))))) {
        iVar5 = map_distance(iVar8,y1,iVar5,iVar9);
        if ((iVar5 < 0xf) && (local_6 < 0xd7 - iVar5)) {
          local_6 = 0xd7;
        }
        else {
          if ((0x27 < iVar5) || (0x5a - iVar5 <= local_6)) goto LAB_5ad0_1463;
          local_6 = 0x5a;
        }
        local_6 = local_6 - iVar5;
        local_4 = uVar11;
      }
      goto LAB_5ad0_1463;
    }
LAB_5ad0_1511:
    iVar9 = iVar5 + -1;
    if (iVar5 != 0) {
      iVar5 = iVar9;
      if (*(char *)(iVar9 * 0x41 + 0x1592) == (char)current_player) {
        iVar4 = map_distance(iVar8,y1,*(int *)(iVar9 * 0x41 + 0x157d),
                             *(int *)(iVar9 * 0x41 + 0x157f));
        if (*(char *)((int)DAT_451b_018e + iVar9 + 0x56) == '\a') {
          iVar4 = iVar4 + -0x50;
        }
        if ((iVar4 < 0xf) && (local_6 < 0x73 - iVar4)) {
          iVar6 = 0x73;
        }
        else if ((0x27 < iVar4) || (iVar6 = local_c, 0x28 - iVar4 <= local_6)) goto LAB_5ad0_1511;
        local_6 = iVar6 - iVar4;
        local_8 = iVar9;
      }
      goto LAB_5ad0_1511;
    }
    if (local_4 != 0xffff) {
      FUN_623c_0ae7(stack,3,local_4,0x100);
      uVar13 = 0;
      move_stack_to();
      iVar12 = FUN_5ad0_15c3(iVar12,local_4,uVar13);
      return iVar12;
    }
    if ((local_8 != -1) &&
       (iVar12 = map_distance(iVar8,y1,*(int *)(local_8 * 0x41 + 0x157d),
                              *(int *)(local_8 * 0x41 + 0x157f)), 2 < iVar12)) {
      FUN_623c_0ae7(stack,1,local_8,0);
      move_stack_to();
    }
  }
  return 0;
}


