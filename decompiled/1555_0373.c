
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far path_wavefront(int *param_1,int param_2)

{
  byte bVar1;
  byte bVar2;
  int iVar3;
  uint *puVar4;
  bool bVar5;
  bool bVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  uint uVar18;
  uint uVar19;
  int *piVar20;
  int *piVar21;
  undefined2 uVar22;
  undefined2 uVar23;
  int local_34;
  int local_2e;
  int local_2c;
  int local_2a;
  int local_28;
  int local_26;
  int local_24;
  int local_22;
  undefined2 local_16;
  int local_14;
  
  biostime(0,0);
  local_16 = 1;
  bVar6 = true;
  local_24 = 0x6f;
  local_26 = 0x9b;
  if ((DAT_4125_00e0 != 0) && (param_2 != 0)) {
    return 0;
  }
  uVar22 = (undefined2)((ulong)param_1 >> 0x10);
  piVar20 = (int *)param_1;
  if ((piVar20[4] & 0x10U) == 0) {
    if (param_2 == 0) {
      DAT_451b_0008 = 0;
      local_22 = 6;
    }
    else if (DAT_4125_00e0 == 0) {
      local_22 = 0x32;
    }
    else {
      local_22 = 0x14;
    }
  }
  else {
    DAT_451b_0008 = 0;
    local_22 = DAT_451b_0010;
  }
  iVar9 = piVar20[2];
  iVar7 = iVar9;
  if (*param_1 <= iVar9) {
    iVar7 = *param_1;
  }
  if (iVar7 < local_22) {
    iVar7 = 0;
  }
  else {
    iVar7 = iVar9;
    if (*param_1 <= iVar9) {
      iVar7 = *param_1;
    }
    iVar7 = iVar7 - local_22;
  }
  iVar10 = piVar20[3];
  iVar8 = iVar10;
  if (piVar20[1] <= iVar10) {
    iVar8 = piVar20[1];
  }
  if (iVar8 < local_22) {
    iVar8 = 0;
  }
  else {
    iVar8 = iVar10;
    if (piVar20[1] <= iVar10) {
      iVar8 = piVar20[1];
    }
    iVar8 = iVar8 - local_22;
  }
  iVar14 = iVar9;
  if (iVar9 <= *param_1) {
    iVar14 = *param_1;
  }
  if (iVar14 + local_22 < 0x70) {
    if (iVar9 <= *param_1) {
      iVar9 = *param_1;
    }
    iVar9 = iVar9 + local_22;
  }
  else {
    iVar9 = 0x6f;
  }
  iVar14 = iVar10;
  if (iVar10 <= piVar20[1]) {
    iVar14 = piVar20[1];
  }
  if (iVar14 + local_22 < 0x9c) {
    if (iVar10 <= piVar20[1]) {
      iVar10 = piVar20[1];
    }
    iVar10 = iVar10 + local_22;
  }
  else {
    iVar10 = 0x9b;
  }
  do {
    if ((!bVar6) || ((bVar5 = false, (piVar20[4] & 0x10U) != 0 && (DAT_451b_0008 == local_22)))) {
      return local_16;
    }
    iVar14 = piVar20[2];
    iVar11 = iVar14 + DAT_451b_0008;
    if (0x6f < iVar11) {
      iVar11 = local_24;
    }
    iVar12 = piVar20[3] + DAT_451b_0008;
    if (0x9b < iVar12) {
      iVar12 = local_26;
    }
    iVar13 = iVar14 - DAT_451b_0008;
    if (iVar14 < DAT_451b_0008) {
      iVar13 = 0;
    }
    local_2c = iVar13 * 0x138;
    local_2e = iVar13 * 0x9c;
    for (; iVar13 <= iVar11; iVar13 = iVar13 + 1) {
      iVar14 = piVar20[3] - DAT_451b_0008;
      if (piVar20[3] < DAT_451b_0008) {
        iVar14 = 0;
      }
      local_2a = iVar14 << 1;
LAB_1555_0880:
      if (iVar14 <= iVar12) {
        if ((((iVar7 <= iVar13) && (iVar13 <= iVar9)) && (iVar8 <= iVar14)) && (iVar14 <= iVar10)) {
          uVar23 = (undefined2)((ulong)_DAT_451b_001a >> 0x10);
          piVar21 = (int *)((int)_DAT_451b_001a + local_2c + local_2a);
          if (*piVar21 < 1) {
            bVar5 = true;
            bVar1 = *(byte *)((int)path_cost_grid + local_2e + iVar14);
            iVar3 = *piVar21;
            iVar15 = -iVar3;
            *piVar21 = iVar15;
            if (DAT_4125_00e0 == 0) {
              local_14 = 0;
            }
            else {
              local_14 = 9;
            }
            if (iVar14 == 0) {
              if (iVar13 == 0) {
                local_14 = 1;
              }
              else if (iVar13 == 0x6f) {
                local_14 = 3;
              }
              else {
                local_14 = 2;
              }
            }
            else if (iVar14 == 0x9b) {
              if (iVar13 == 0) {
                local_14 = 6;
              }
              else if (iVar13 == 0x6f) {
                local_14 = 8;
              }
              else {
                local_14 = 7;
              }
            }
            else if (iVar13 == 0) {
              local_14 = 4;
            }
            else if (iVar13 == 0x6f) {
              local_14 = 5;
            }
            local_34 = *(int *)(local_14 * 2 + 0xe2);
            local_28 = local_34 << 1;
LAB_1555_0853:
            do {
              if (*(int *)(local_28 + 0xf6) == -10) goto LAB_1555_0860;
              iVar16 = iVar13 + *(int *)(local_28 + 0xf6);
              iVar17 = iVar14 + *(int *)(local_28 + 0x162);
              local_28 = local_28 + 2;
              local_34 = local_34 + 1;
              puVar4 = (uint *)CONCAT22((int)((ulong)_DAT_451b_001a >> 0x10),
                                        (uint *)(DAT_451b_001a + iVar16 * 0x138 + iVar17 * 2));
              if ((((iVar13 < 0) || (iVar14 < 0)) ||
                  ((0x6f < iVar13 || ((0x9b < iVar14 || (iVar16 < 0)))))) ||
                 ((iVar17 < 0 || ((0x6f < iVar16 || (0x9b < iVar14)))))) {
                sprintf(&DAT_451b_1d28,s_ERROR_____d___d____d___d___d__d_4125_0242,iVar13,iVar14,
                        iVar16,iVar17,local_14,local_34);
                FUN_5c52_0478((undefined1 *)&DAT_451b_1d28,0x4125);
              }
              if (*puVar4 != 0x7531) {
                bVar2 = *(byte *)((int)path_cost_grid + iVar16 * 0x9c + iVar17);
                uVar18 = bVar2 & 7;
                if ((*param_1 == iVar16) && (piVar20[1] == iVar17)) {
                  if (((piVar20[5] == 1) && ((bVar1 & 0x10) == 0)) &&
                     (((bVar2 & 0x10) == 0 && ((bVar2 & 8) != (bVar1 & 8))))) goto LAB_1555_0853;
                  uVar18 = 1;
                }
                else if (piVar20[5] == 2) {
                  if (((bVar2 & 8) == 0) || ((bVar2 & 0x10) != 0)) {
                    uVar19 = uVar18;
                    if ((bVar2 & 7) == 0) {
                      uVar19 = 2;
                    }
                    if (uVar19 < 3) {
                      if ((bVar2 & 7) == 0) {
                        uVar18 = 2;
                      }
                    }
                    else {
                      uVar18 = 2;
                    }
                  }
                  else {
                    uVar18 = 2;
                  }
                }
                else if (piVar20[5] == 0) {
                  if (((bVar2 & 8) == 0) && ((bVar2 & 0x80) == 0)) {
                    *puVar4 = 0x7531;
                    goto LAB_1555_0853;
                  }
                }
                else {
                  if ((((bVar1 & 0x10) == 0) && ((bVar2 & 0x10) == 0)) &&
                     ((bVar2 & 8) != (bVar1 & 8))) goto LAB_1555_0853;
                  if (((piVar20[4] != 0) && (2 < uVar18)) &&
                     ((((piVar20[4] & 1U) != 0 && ((bVar2 & 0x20) != 0)) ||
                      (((piVar20[4] & 2U) != 0 && ((bVar2 & 0x40) != 0)))))) {
                    uVar18 = 2;
                  }
                  if (((((bVar1 & 8) == 0) || ((bVar1 & 0x10) != 0)) && ((bVar2 & 8) != 0)) &&
                     ((bVar2 & 0x10) == 0)) {
                    uVar18 = uVar18 + DAT_451b_000e;
                  }
                }
                uVar19 = (int)*puVar4 >> 0xf;
                if ((int)(iVar15 + uVar18) < (int)((*puVar4 ^ uVar19) - uVar19)) {
                  *puVar4 = iVar3 - uVar18;
                }
              }
            } while( true );
          }
        }
        goto LAB_1555_087b;
      }
      local_2c = local_2c + 0x138;
      local_2e = local_2e + 0x9c;
    }
    DAT_451b_0008 = DAT_451b_0008 + 1;
    if (!bVar5) {
      bVar6 = false;
      local_16 = 0;
    }
  } while( true );
LAB_1555_0860:
  if (((*param_1 == iVar13) && (piVar20[1] == iVar14)) && ((piVar20[4] & 0x10U) == 0)) {
    bVar6 = false;
  }
LAB_1555_087b:
  local_2a = local_2a + 2;
  iVar14 = iVar14 + 1;
  goto LAB_1555_0880;
}


