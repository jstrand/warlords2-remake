
void __cdecl16far ai_phase_diplomacy(void)

{
  byte *pbVar1;
  int *piVar2;
  bool bVar3;
  char cVar4;
  int *piVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  uint uVar12;
  undefined2 uVar13;
  undefined2 unaff_SS;
  int local_3a [8];
  int local_2a [8];
  int local_1a [8];
  int local_a;
  int local_8;
  uint local_6;
  int local_4;
  
  local_6 = (uint)(*(int *)(current_player * 2 + 0xd0) == 0);
  FUN_558d_0917();
  if (opt_diplomacy == 0) {
    return;
  }
  DAT_451b_012e = ai_pick_enemy();
  iVar8 = 8;
LAB_558d_0101:
  iVar9 = iVar8 + -1;
  if (iVar8 != 0) {
    pbVar1 = (byte *)(current_player * 8 + iVar9 + 0x153b);
    *pbVar1 = *pbVar1 & 0xf3;
    *(undefined2 *)(iVar9 * 2 + 0x4090) = 0;
    *(undefined2 *)(iVar9 * 2 + 0x40a0) = 0;
    *(undefined2 *)(iVar9 * 2 + 0x40b0) = 0;
    local_2a[iVar9] = 0;
    local_1a[iVar9] = 0;
    iVar8 = iVar9;
    if ((*(byte *)(iVar9 * 8 + current_player + 0x153b) & 0xc) != 0) goto LAB_558d_00cd;
    piVar5 = local_1a;
    goto LAB_558d_00fa;
  }
  iVar9 = 0;
  iVar8 = city_count;
LAB_558d_0226:
  do {
    do {
      iVar7 = iVar8 + -1;
      if (iVar8 == 0) {
        uVar6 = 8;
        while (uVar10 = uVar6 - 1, uVar6 != 0) {
          uVar6 = uVar10;
          if ((current_player != uVar10) && (*(int *)(uVar10 * 2 + 0x137) != 0)) {
            iVar8 = (local_1a[uVar10] - local_2a[uVar10]) * 2;
            if (*(int *)(uVar10 * 2 + 0x4090) < iVar8 + 4) {
              if (iVar8 + 8 <= *(int *)(uVar10 * 2 + 0x4090)) {
                iVar8 = current_player * 8 + uVar10;
                pbVar1 = (byte *)(iVar8 + 0x153b);
                *pbVar1 = *pbVar1 & 0xf3;
                pbVar1 = (byte *)(iVar8 + 0x153b);
                *pbVar1 = *pbVar1 | 8;
              }
            }
            else {
              iVar8 = current_player * 8 + uVar10;
              pbVar1 = (byte *)(iVar8 + 0x153b);
              *pbVar1 = *pbVar1 & 0xf3;
              pbVar1 = (byte *)(iVar8 + 0x153b);
              *pbVar1 = *pbVar1 | 4;
            }
          }
        }
        uVar6 = 8;
        while (uVar10 = uVar6 - 1, uVar6 != 0) {
          uVar6 = uVar10;
          if ((current_player != uVar10) && (*(int *)(uVar10 * 2 + 0x137) != 0)) {
            uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
            iVar8 = (int)DAT_451b_018e + uVar10 * 2;
            iVar8 = *(int *)(iVar8 + 0x3ec) + *(int *)(iVar8 + 0x40c) * 4 +
                    *(int *)(iVar8 + 0x3bc) * 2;
            iVar7 = (local_1a[uVar10] - local_2a[uVar10]) * 2;
            if (iVar8 < iVar7 + 5) {
              if (iVar7 + 10 <= iVar8) {
                iVar8 = current_player * 8 + uVar10;
                pbVar1 = (byte *)(iVar8 + 0x153b);
                *pbVar1 = *pbVar1 & 0xf3;
                pbVar1 = (byte *)(iVar8 + 0x153b);
                *pbVar1 = *pbVar1 | 8;
              }
            }
            else {
              if ((*(byte *)(current_player * 8 + uVar10 + 0x153b) & 0xc) >> 2 < 2) {
                cVar4 = '\x01';
              }
              else {
                cVar4 = (char)((*(byte *)(current_player * 8 + uVar10 + 0x153b) & 0xc) >> 2);
              }
              iVar8 = current_player * 8 + uVar10;
              pbVar1 = (byte *)(iVar8 + 0x153b);
              *pbVar1 = *pbVar1 & 0xf3;
              pbVar1 = (byte *)(iVar8 + 0x153b);
              *pbVar1 = *pbVar1 | cVar4 << 2;
            }
          }
        }
        if (DAT_451b_012e != 0xffff) {
          iVar8 = current_player * 8 + DAT_451b_012e;
          pbVar1 = (byte *)(iVar8 + 0x153b);
          *pbVar1 = *pbVar1 & 0xf3;
          pbVar1 = (byte *)(iVar8 + 0x153b);
          *pbVar1 = *pbVar1 | 8;
        }
        iVar8 = 4;
        while( true ) {
          iVar11 = iVar8 + -1;
          iVar7 = (int)DAT_451b_018e;
          uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
          if (iVar8 == 0) break;
          iVar8 = iVar11;
          if (*(int *)(iVar7 + iVar11 * 0x5c + 0x24c) != 0) {
            iVar7 = current_player * 8 + *(int *)(iVar7 + iVar11 * 0x5c + 0x24e);
            pbVar1 = (byte *)(iVar7 + 0x153b);
            *pbVar1 = *pbVar1 & 0xf3;
            pbVar1 = (byte *)(iVar7 + 0x153b);
            *pbVar1 = *pbVar1 | 8;
          }
        }
        local_4 = 0;
        uVar6 = 0xffff;
        uVar10 = 8;
        while (uVar12 = uVar10 - 1, uVar10 != 0) {
          uVar10 = uVar12;
          if (*(int *)(uVar12 * 2 + 0x137) != 0) {
            local_8 = iVar9;
            if (iVar9 < 1) {
              local_8 = 1;
            }
            local_8 = (*(int *)(uVar12 * 2 + 0x40a0) * 100) / local_8;
            if (*(int *)(uVar12 * 2 + 0xd0) == 0) {
              local_a = *(int *)(iVar7 + 0x44);
            }
            else {
              local_a = 0x32;
            }
            if ((local_4 < *(int *)(uVar12 * 2 + 0x40a0)) && (local_a < local_8)) {
              local_4 = *(int *)(uVar12 * 2 + 0x40a0);
              uVar6 = uVar12;
            }
          }
        }
        if ((uVar6 != 0xffff) && (current_player != uVar6)) {
          uVar10 = 8;
          while (uVar12 = uVar10 - 1, uVar10 != 0) {
            uVar10 = uVar12;
            if (*(int *)(uVar12 * 2 + 0x137) != 0) {
              if ((uVar12 == uVar6) || (uVar12 == DAT_451b_012e)) {
                iVar8 = current_player * 8 + uVar12;
                pbVar1 = (byte *)(iVar8 + 0x153b);
                *pbVar1 = *pbVar1 & 0xf3;
                pbVar1 = (byte *)(iVar8 + 0x153b);
                *pbVar1 = *pbVar1 | 8;
              }
              else {
                pbVar1 = (byte *)(current_player * 8 + uVar12 + 0x153b);
                *pbVar1 = *pbVar1 & 0xf3;
              }
            }
          }
        }
        if ((local_6 == 0) && (*(int *)((int)DAT_451b_018e + 0x38) != 0)) {
          bVar3 = false;
          uVar6 = 8;
          while (uVar10 = uVar6 - 1, uVar6 != 0) {
            uVar6 = uVar10;
            if ((((*(int *)(uVar10 * 2 + 0x137) != 0) && (*(int *)(uVar10 * 2 + 0xd0) != 0)) &&
                (current_player != uVar10)) &&
               (((*(byte *)(current_player * 8 + uVar10 + 0x153b) & 0xc) != 0 &&
                (iVar8 = FUN_558d_0a6e(uVar10), iVar8 != 0)))) {
              pbVar1 = (byte *)(current_player * 8 + uVar10 + 0x153b);
              *pbVar1 = *pbVar1 & 0xf3;
            }
          }
          iVar8 = 8;
          while (iVar9 = iVar8 + -1, iVar8 != 0) {
            iVar8 = iVar9;
            if (((*(int *)(iVar9 * 2 + 0x137) != 0) && (*(int *)(iVar9 * 2 + 0xd0) != 1)) &&
               ((*(byte *)(current_player * 8 + iVar9 + 0x153b) & 3) == 2)) {
              bVar3 = true;
            }
          }
          if (bVar3) {
            iVar8 = 8;
            while (iVar9 = iVar8 + -1, iVar8 != 0) {
              iVar8 = iVar9;
              if (((*(int *)(iVar9 * 2 + 0x137) != 0) && (*(int *)(iVar9 * 2 + 0xd0) == 1)) &&
                 (*(int *)(iVar9 * 2 + 0x40f2) != 0)) {
                pbVar1 = (byte *)(current_player * 8 + iVar9 + 0x153b);
                *pbVar1 = *pbVar1 & 0xf3;
              }
            }
          }
        }
        if ((local_6 == 0) && (opt_all_computer != 0)) {
          iVar8 = 8;
          while (iVar8 != 0) {
            local_3a[iVar8 + -1] = 0;
            iVar8 = iVar8 + -1;
          }
          iVar8 = 8;
          while (iVar9 = iVar8 + -1, iVar8 != 0) {
            iVar8 = iVar9;
            if ((*(int *)(iVar9 * 2 + 0x137) != 0) && (*(int *)(iVar9 * 2 + 0xd0) != 0)) {
              iVar7 = 8;
              while (iVar11 = iVar7 + -1, iVar7 != 0) {
                iVar7 = iVar11;
                if (((*(int *)(iVar11 * 2 + 0x137) != 0) && (*(int *)(iVar11 * 2 + 0xd0) != 1)) &&
                   ((*(byte *)(iVar9 * 8 + iVar11 + 0x153b) & 3) == 2)) {
                  local_3a[iVar9] = 1;
                }
              }
            }
          }
          iVar8 = 8;
          while (iVar9 = iVar8 + -1, iVar8 != 0) {
            iVar8 = iVar9;
            if (((*(int *)(iVar9 * 2 + 0x137) != 0) && (*(int *)(iVar9 * 2 + 0xd0) != 0)) &&
               ((local_3a[current_player] != 0 || (local_3a[iVar9] != 0)))) {
              pbVar1 = (byte *)(current_player * 8 + iVar9 + 0x153b);
              *pbVar1 = *pbVar1 & 0xf3;
            }
          }
        }
        iVar8 = 8;
        while (iVar9 = iVar8 + -1, iVar8 != 0) {
          iVar8 = iVar9;
          if (*(int *)(iVar9 * 2 + 0x40b0) == 0) {
            pbVar1 = (byte *)(current_player * 8 + iVar9 + 0x153b);
            *pbVar1 = *pbVar1 & 0xf3;
          }
        }
        iVar8 = 4;
        while (iVar9 = iVar8 + -1, iVar8 != 0) {
          uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
          iVar8 = iVar9;
          if ((*(int *)((int)DAT_451b_018e + iVar9 * 0x5c + 0x24c) != 0) &&
             ((*(byte *)(current_player * 8 + *(int *)((int)DAT_451b_018e + iVar9 * 0x5c + 0x24e) +
                        0x153b) & 0xc) == 0)) {
            FUN_563e_066d(iVar9,0x14);
          }
        }
        return;
      }
      if (*(char *)(*(byte *)(*(int *)(iVar7 * 0x41 + 0x157f) * 0xe0 +
                             *(int *)(iVar7 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
        iVar9 = iVar9 + 1;
      }
      iVar8 = iVar7;
    } while (*(char *)(iVar7 * 0x41 + 0x1592) == '\x0f');
    piVar2 = (int *)(*(char *)(iVar7 * 0x41 + 0x1592) * 2 + 0x40a0);
    *piVar2 = *piVar2 + 1;
    if ((*(byte *)((int)DAT_451b_018e + iVar7 + 0x11e) & 1) == 0) {
      piVar2 = (int *)(*(char *)(iVar7 * 0x41 + 0x1592) * 2 + 0x40b0);
      *piVar2 = *piVar2 + 1;
    }
    if ((int)*(char *)(iVar7 * 0x41 + 0x1592) != current_player) goto LAB_558d_01f1;
  } while (*(char *)(iVar7 * 0x41 + 0x15ac) == '\x0f');
  uVar6 = (uint)*(byte *)(iVar7 * 0x41 + 0x15ac);
  goto LAB_558d_021e;
LAB_558d_00cd:
  if ((*(byte *)(iVar9 * 8 + current_player + 0x153b) & 0xc) == 8) {
    piVar5 = local_2a;
LAB_558d_00fa:
    piVar5[iVar9] = 1;
  }
  goto LAB_558d_0101;
LAB_558d_01f1:
  if (*(byte *)(iVar7 * 0x41 + 0x15ac) == current_player) {
    uVar6 = (uint)*(char *)(iVar7 * 0x41 + 0x1592);
LAB_558d_021e:
    piVar2 = (int *)(uVar6 * 2 + 0x4090);
    *piVar2 = *piVar2 + 1;
  }
  goto LAB_558d_0226;
}


