
void __cdecl16far FUN_59bf_01b3(void)

{
  byte *pbVar1;
  uint *puVar2;
  bool bVar3;
  undefined2 uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int unaff_DI;
  undefined2 uVar13;
  undefined2 unaff_SS;
  int local_26 [8];
  int local_16;
  int local_14;
  uint local_12;
  int local_10;
  uint local_a;
  uint local_8;
  int local_6;
  int local_4;
  
  local_10 = 0;
  uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar10 = (int)DAT_451b_018e;
  iVar12 = *(int *)(iVar10 + 0x24a);
  while (iVar5 = iVar12 + -1, iVar12 != 0) {
    iVar12 = iVar5;
    if (*(int *)(iVar10 + iVar5 * 0x5c + 0x24c) != 0) {
      local_10 = local_10 + 1;
    }
  }
  *(undefined2 *)(iVar10 + 0x40) = 0;
  *(undefined2 *)(iVar10 + 0x3e) = 0;
  *(undefined2 *)(iVar10 + 0x3c) = 0;
  *(undefined2 *)(iVar10 + 0x3a) = 0;
  iVar12 = 8;
  while (iVar10 = city_count, iVar12 != 0) {
    local_26[iVar12 + -1] = 0;
    iVar12 = iVar12 + -1;
  }
  while( true ) {
    iVar12 = iVar10 + -1;
    uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (iVar10 == 0) break;
    iVar10 = iVar12;
    if (*(char *)(iVar12 * 0x41 + 0x1592) == current_player) {
      pbVar1 = (byte *)((int)DAT_451b_018e + iVar12 + 0x11e);
      *pbVar1 = *pbVar1 & 0x37;
      if ((*(char *)((int)DAT_451b_018e + iVar12 + 0x56) != '\a') &&
         (iVar5 = FUN_623c_1a4d(iVar12,&local_4), iVar5 != 0)) {
        local_6 = -1;
        iVar5 = (int)*(char *)(iVar12 * 0x41 + local_4 + 0x159b);
        if (((undefined1 *)&DAT_451b_0669)[*(char *)(iVar12 * 0x41 + local_4 + 0x1593) * 6] ==
            '\x01') {
          iVar5 = iVar5 + 2;
        }
        iVar12 = 8;
        while (iVar8 = iVar12 + -1, iVar12 != 0) {
          if (local_26[iVar8] == 0) {
            local_26[iVar8] = iVar5;
            local_6 = -1;
            break;
          }
          iVar12 = iVar8;
          if (local_26[iVar8] < unaff_DI) {
            unaff_DI = local_26[iVar8];
            local_6 = iVar8;
          }
        }
        unaff_DI = iVar5;
        if (local_6 != -1) {
          local_26[local_6] = iVar5;
        }
      }
    }
  }
  iVar12 = 100;
  iVar10 = 8;
  while (iVar5 = iVar10 + -1, iVar10 != 0) {
    iVar10 = iVar5;
    if ((local_26[iVar5] != 0) && (local_26[iVar5] < iVar12)) {
      iVar12 = local_26[iVar5];
    }
  }
  if (4 < iVar12) {
    iVar12 = 4;
  }
  *(int *)((int)DAT_451b_018e + 0x3a) = iVar12;
  iVar12 = city_count;
  if (local_10 == 4) {
    while (iVar10 = iVar12 + -1, iVar12 != 0) {
      iVar12 = iVar10;
      if ((*(char *)(iVar10 * 0x41 + 0x1592) == current_player) &&
         (iVar5 = FUN_5f19_0a7a(iVar10,1), iVar5 != 0)) {
        uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        iVar5 = (int)DAT_451b_018e;
        if ((*(byte *)(iVar5 + iVar10 + 0x11e) & 2) != 0) {
          bVar3 = false;
          iVar8 = 4;
          while (iVar9 = iVar8 + -1, iVar8 != 0) {
            local_16 = (int)*(char *)(iVar10 * 0x41 + iVar9 + 0x1593);
            iVar8 = iVar9;
            if (((-1 < local_16) && (((undefined1 *)&army_move_flags)[local_16 * 6] != '\0')) &&
               ('\x04' < *(char *)(iVar10 * 0x41 + iVar9 + 0x159b))) {
              bVar3 = true;
            }
          }
          if (bVar3) {
            *(int *)(iVar5 + 0x3c) = *(int *)(iVar5 + 0x3c) + 1;
            pbVar1 = (byte *)(iVar5 + iVar10 + 0x11e);
            *pbVar1 = *pbVar1 | 0x40;
          }
        }
      }
    }
  }
  uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar12 = (int)DAT_451b_018e;
  bVar3 = *(int *)(iVar12 + 0x3c) < 4;
  if (bVar3) {
    iVar12 = *(int *)(iVar12 + 0x3c);
  }
  else {
    iVar12 = *(int *)(iVar12 + 0x3c) + -4;
  }
  local_8 = (uint)!bVar3;
  iVar10 = city_count;
  if (2 < local_10) {
LAB_59bf_052d:
    iVar5 = iVar10 + -1;
    if (iVar10 != 0) {
      iVar10 = iVar5;
      if ((*(char *)(iVar5 * 0x41 + 0x1592) == current_player) &&
         (iVar8 = FUN_5f19_0a7a(iVar5,1), iVar8 != 0)) {
        uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        if ((*(byte *)((int)DAT_451b_018e + iVar5 + 0x11e) & 0x40) != 0) {
          if (iVar12 == 0) goto LAB_59bf_052d;
          pbVar1 = (byte *)((int)DAT_451b_018e + iVar5 + 0x11e);
          *pbVar1 = *pbVar1 & 0xbf;
          iVar12 = iVar12 + -1;
        }
        bVar3 = false;
        iVar8 = 4;
        while( true ) {
          iVar9 = iVar8 + -1;
          iVar11 = (int)DAT_451b_018e;
          uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
          if (iVar8 == 0) break;
          iVar8 = iVar9;
          if (((-1 < *(char *)(iVar5 * 0x41 + iVar9 + 0x1593)) &&
              ((((undefined1 *)&DAT_451b_0669)[*(char *)(iVar5 * 0x41 + iVar9 + 0x1593) * 6] ==
                '\x01' || (*(int *)(iVar11 + 0x3a) <= (int)*(char *)(iVar5 * 0x41 + iVar9 + 0x159b))
               ))) && ('\x0f' < *(char *)(iVar5 * 0x41 + iVar9 + 0x159f))) {
            bVar3 = true;
          }
        }
        if (bVar3) {
          *(int *)(iVar11 + 0x3e) = *(int *)(iVar11 + 0x3e) + 1;
          pbVar1 = (byte *)(iVar11 + iVar5 + 0x11e);
          *pbVar1 = *pbVar1 | 0x80;
        }
      }
      goto LAB_59bf_052d;
    }
  }
  uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar12 = (int)DAT_451b_018e;
  local_12 = (uint)(7 < *(int *)(iVar12 + 0x3e));
  local_14 = 0;
  bVar3 = *(int *)(iVar12 + 0x3e) < 4;
  if (bVar3) {
    iVar12 = *(int *)(iVar12 + 0x3e);
  }
  else {
    iVar12 = *(int *)(iVar12 + 0x3e) + -4;
  }
  local_a = (uint)!bVar3;
  iVar10 = city_count;
  if (1 < local_10) {
LAB_59bf_0692:
    iVar5 = iVar10 + -1;
    if (iVar10 != 0) {
      iVar10 = iVar5;
      if ((*(char *)(iVar5 * 0x41 + 0x1592) == current_player) &&
         (iVar8 = FUN_5f19_0a7a(iVar5,1), iVar8 != 0)) {
        uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        iVar8 = (int)DAT_451b_018e;
        if ((*(byte *)(iVar8 + iVar5 + 0x11e) & 0x40) == 0) {
          if ((*(byte *)(iVar8 + iVar5 + 0x11e) & 0x80) != 0) {
            if (iVar12 == 0) goto LAB_59bf_0692;
            pbVar1 = (byte *)(iVar8 + iVar5 + 0x11e);
            *pbVar1 = *pbVar1 & 0x7f;
            iVar12 = iVar12 + -1;
          }
          bVar3 = false;
          iVar8 = 4;
          while( true ) {
            iVar9 = iVar8 + -1;
            iVar11 = (int)DAT_451b_018e;
            uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
            if (iVar8 == 0) break;
            iVar8 = iVar9;
            if ((-1 < *(char *)(iVar5 * 0x41 + iVar9 + 0x1593)) &&
               ((((((undefined1 *)&DAT_451b_0669)[*(char *)(iVar5 * 0x41 + iVar9 + 0x1593) * 6] ==
                   '\x01' ||
                  (*(int *)(iVar11 + 0x3a) <= (int)*(char *)(iVar5 * 0x41 + iVar9 + 0x159b))) &&
                 ('\v' < *(char *)(iVar5 * 0x41 + iVar9 + 0x159f))) &&
                ((local_14 = local_14 + 1, local_12 == 0 ||
                 ('\x0f' < *(char *)(iVar5 * 0x41 + iVar9 + 0x159f))))))) {
              bVar3 = true;
            }
          }
          if (bVar3) {
            *(int *)(iVar11 + 0x40) = *(int *)(iVar11 + 0x40) + 1;
            pbVar1 = (byte *)(iVar11 + iVar5 + 0x11e);
            *pbVar1 = *pbVar1 | 8;
          }
        }
      }
      goto LAB_59bf_0692;
    }
  }
  uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar12 = (int)DAT_451b_018e;
  bVar3 = *(int *)(iVar12 + 0x40) < 4;
  if (bVar3) {
    iVar12 = *(int *)(iVar12 + 0x40);
  }
  else {
    iVar12 = *(int *)(iVar12 + 0x40) + -4;
  }
  iVar10 = city_count;
  while( true ) {
    iVar8 = iVar10 + -1;
    iVar5 = (int)DAT_451b_018e;
    uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (iVar10 == 0) break;
    iVar10 = iVar8;
    if ((((*(char *)(iVar8 * 0x41 + 0x1592) == current_player) &&
         ((*(byte *)(iVar5 + iVar8 + 0x11e) & 0xc0) == 0)) &&
        ((*(byte *)(iVar5 + iVar8 + 0x11e) & 8) != 0)) && (iVar12 != 0)) {
      pbVar1 = (byte *)(iVar5 + iVar8 + 0x11e);
      *pbVar1 = *pbVar1 & 0xf7;
      iVar12 = iVar12 + -1;
    }
  }
  uVar6 = *(uint *)(iVar5 + 0x24a);
LAB_59bf_0837:
  do {
    do {
      uVar7 = uVar6 - 1;
      if (uVar6 == 0) {
        return;
      }
      uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      uVar6 = uVar7;
    } while ((*(int *)((int)DAT_451b_018e + uVar7 * 0x5c + 0x24c) == 0) ||
            (*(undefined2 *)((int)DAT_451b_018e + uVar7 * 0x5c + 0x2a4) = 8, 3 < uVar7));
                    /* WARNING: Switch is manually overridden */
    iVar12 = (int)DAT_451b_018e;
    uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    switch((undefined4)*(undefined2 *)(uVar7 * 2 + 0x845)) {
    case 0x5a363:
      if (local_14 < 8) {
        uVar4 = 8;
      }
      else {
        uVar4 = 0xc;
      }
      *(undefined2 *)(iVar12 + uVar7 * 0x5c + 0x2a4) = uVar4;
      break;
    case 0x5a384:
      puVar2 = (uint *)(iVar12 + uVar7 * 0x5c + 0x2a6);
      *puVar2 = *puVar2 & 0xffef;
      if (!bVar3) {
        puVar2 = (uint *)((int)DAT_451b_018e + uVar7 * 0x5c + 0x2a6);
        *puVar2 = *puVar2 | 0x10;
        goto LAB_59bf_0825;
      }
      break;
    case 0x5a3b1:
      puVar2 = (uint *)(iVar12 + uVar7 * 0x5c + 0x2a6);
      *puVar2 = *puVar2 & 0xfff7;
      if (local_a != 0) {
        uVar13 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        puVar2 = (uint *)((int)DAT_451b_018e + uVar7 * 0x5c + 0x2a6);
        *puVar2 = *puVar2 | 8;
        *(undefined2 *)((int)DAT_451b_018e + uVar7 * 0x5c + 0x2a4) = 0x10;
      }
      break;
    case 0x5a3ed:
      goto override_jmp_59bf_076e_case_3;
    }
  } while( true );
override_jmp_59bf_076e_case_3:
  puVar2 = (uint *)(iVar12 + uVar7 * 0x5c + 0x2a6);
  *puVar2 = *puVar2 & 0xffdf;
  if (local_8 != 0) {
    puVar2 = (uint *)((int)DAT_451b_018e + uVar7 * 0x5c + 0x2a6);
    *puVar2 = *puVar2 | 0x20;
LAB_59bf_0825:
    *(undefined2 *)((int)DAT_451b_018e + uVar7 * 0x5c + 0x2a4) = 0xc;
  }
  goto LAB_59bf_0837;
}


