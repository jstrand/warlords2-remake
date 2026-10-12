
void __cdecl16far auto_str_legal_site_dist_current(int param_1,undefined4 param_2)

{
  int iVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  uint uVar11;
  undefined2 uVar12;
  int local_10;
  uint local_e;
  int local_c;
  uint local_a;
  
  uVar12 = (undefined2)((ulong)param_2 >> 0x10);
  iVar10 = (int)param_2;
  iVar1 = *(int *)(iVar10 + param_1 * 2);
  local_a = 0xffff;
  local_c = 100;
  local_e = 0xffff;
  local_10 = 100;
  if ((*(byte *)(iVar1 * 0x16 + -0x7772) & 0xf) == 1) {
    iVar4 = 5;
  }
  else {
    iVar4 = 0x19;
  }
  uVar11 = 0;
  do {
    if (site_count <= (int)uVar11) {
      *(uint *)(iVar10 + 0x30) = local_e;
      *(int *)(iVar10 + 0x32) = local_10;
      *(uint *)(iVar10 + 0x34) = local_a;
      *(int *)(iVar10 + 0x36) = local_c;
      return;
    }
    iVar6 = *(int *)(uVar11 * 0x1f + 0x811);
    iVar8 = *(int *)(uVar11 * 0x1f + 0x813);
    bVar2 = true;
    iVar5 = FUN_6087_1637(uVar11,iVar1);
    if (iVar5 != 0) {
      iVar6 = map_distance(iVar6,iVar8,*(int *)(iVar1 * 0x16 + -0x7780),
                           *(int *)(iVar1 * 0x16 + -0x777e));
      if (*(char *)(uVar11 * 0x1f + 0x829) == '\x01') {
        if ((((opt_quests == 0) || (*(int *)(current_player * 0xc + 0x1103) != 0)) ||
            ((iVar4 <= iVar6 || (iVar5 = iVar6, uVar3 = uVar11, local_10 <= iVar6)))) &&
           (uVar7 = FUN_6087_063c(uVar11), iVar5 = local_10, uVar3 = local_e,
           (*(uint *)(iVar1 * 0x16 + -0x7774) & uVar7) != 0)) goto LAB_6087_11c5;
      }
      else {
        iVar8 = 6;
        while (iVar9 = iVar8 + -1, iVar5 = local_10, uVar3 = local_e, iVar8 != 0) {
          iVar8 = iVar9;
          if ((((iVar9 != param_1) && (*(int *)(iVar10 + iVar9 * 2) != -1)) &&
              ((*(byte *)(*(int *)(iVar10 + iVar9 * 2) * 0x16 + -0x7772) & 0xf) == 3)) &&
             (*(byte *)(*(int *)(iVar10 + iVar9 * 2) * 0x16 + -0x7771) >> 1 == uVar11)) {
            bVar2 = false;
          }
        }
      }
      local_e = uVar3;
      local_10 = iVar5;
      if ((bVar2) && (iVar6 <= iVar4)) {
        if (iVar6 < local_c) {
          local_c = iVar6;
          local_a = uVar11;
        }
        sprintf(&DAT_451b_1d28,s_Legal_site__d__dist__d___d___cur_4125_09c6,uVar11,iVar6,iVar4,
                local_a);
        FUN_623c_170c((undefined1 *)&DAT_451b_1d28,0x4125,*(undefined2 *)(uVar11 * 0x1f + 0x811),
                      *(undefined2 *)(uVar11 * 0x1f + 0x813));
      }
    }
LAB_6087_11c5:
    uVar11 = uVar11 + 1;
  } while( true );
}


