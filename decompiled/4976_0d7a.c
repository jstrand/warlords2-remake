
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far quest_assign(int param_1)

{
  uint *puVar1;
  bool bVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int unaff_DI;
  int iStack_10;
  int local_e;
  int iStack_c;
  int iStack_a;
  int iStack_8;
  int iStack_6;
  uint local_4;
  
  local_e = 0;
  FUN_6d51_1244(current_player,3,0,(char)((int *)_DAT_451b_1efe)[5] * 0x14 + 0x223,0x3c04);
LAB_4976_0db6:
  do {
    local_e = local_e + 1;
    uVar3 = FUN_1b62_0044(_DAT_451b_1efe);
    if (param_1 == 0) {
      iVar4 = dice(1,10,-1);
      local_4 = *(uint *)(iVar4 * 2 + 0xa0);
    }
    else if (local_e == 1) {
      iVar4 = dice(1,5,-1);
      if (iVar4 == 0) {
        local_4 = 5;
      }
      else {
        local_4 = 4;
      }
    }
    else if (local_e == 2) {
      local_4 = 3;
    }
    else if (local_e == 3) {
      local_4 = 6;
    }
    *(undefined2 *)(current_player * 0xc + 0x1103) = 1;
    *(undefined2 *)(current_player * 0xc + 0x1107) = uVar3;
    *(uint *)(current_player * 0xc + 0x1105) = local_4;
    if (6 < local_4) {
      return;
    }
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)(local_4 * 2 + 0x1512)) {
    case 0x4a5e8:
      iStack_10 = 0;
      iVar4 = army_count;
      while (iVar6 = iVar4 + -1, iVar4 != 0) {
        iVar4 = iVar6;
        if ((*(char *)(iVar6 * 0x16 + -0x777c) == '\x1c') &&
           (*(char *)(iVar6 * 0x16 + -0x777b) != current_player)) {
          iStack_10 = iStack_10 + 1;
        }
      }
      if (0 < iStack_10) {
        iVar6 = dice(1,iStack_10,0);
        iStack_10 = 0;
        iVar4 = army_count;
        goto LAB_4976_0f32;
      }
      goto LAB_4976_0db6;
    case 0x4a6b3:
      iStack_10 = 0;
      for (iStack_6 = 0; iStack_6 < 0x16; iStack_6 = iStack_6 + 1) {
        if (param_1 == 0) {
          iVar4 = 0x32;
        }
        else {
          iVar4 = 0x1e;
        }
        FUN_66d4_0b36(iStack_6,&iStack_8);
        if (*(char *)(iStack_6 * 0x1d + 0xcff) == '\x02') {
          __F_SPUSH();
          iVar6 = item_reserved();
          if (iVar6 == 0) {
            uVar7 = *_DAT_451b_1efe - iStack_8 >> 0xf;
            if (((int)((*_DAT_451b_1efe - iStack_8 ^ uVar7) - uVar7) < iVar4) &&
               (uVar7 = ((int *)_DAT_451b_1efe)[1] - iStack_a, uVar8 = (int)uVar7 >> 0xf,
               (int)((uVar7 ^ uVar8) - uVar8) < iVar4)) {
              iStack_10 = iStack_10 + 1;
            }
          }
        }
      }
      if (0 < iStack_10) {
        iVar4 = dice(1,iStack_10,0);
        iStack_6 = 0;
        iStack_10 = 0;
        goto LAB_4976_1097;
      }
      goto LAB_4976_0db6;
    case 0x4a89a:
      bVar2 = false;
      iStack_6 = 0;
      while ((iStack_6 < 5 && (!bVar2))) {
        unaff_DI = random_magical_type();
        bVar2 = false;
        iVar4 = army_count;
        do {
          iVar6 = iVar4 + -1;
          if (iVar4 == 0) goto LAB_4976_11c0;
          iVar4 = iVar6;
        } while ((((*(char *)(iVar6 * 0x16 + -0x777c) != unaff_DI) ||
                  (*(char *)(iVar6 * 0x16 + -0x777b) == current_player)) ||
                 (*(int *)(iVar6 * 0x16 + -0x7780) < 0)) || (*(int *)(iVar6 * 0x16 + -0x777e) < 0));
        bVar2 = true;
LAB_4976_11c0:
        iStack_6 = iStack_6 + 1;
      }
      if (bVar2) goto LAB_4976_149c;
      goto LAB_4976_0db6;
    case 0x4a93e:
      if (game_won == 0) {
        bVar2 = false;
        for (iStack_c = 0; (!bVar2 && (iStack_c < 200)); iStack_c = iStack_c + 1) {
          unaff_DI = dice(1,8,-1);
          if ((current_player != unaff_DI) && (*(int *)(unaff_DI * 2 + 0x137) != 0)) {
            bVar2 = true;
          }
        }
        if (iStack_c < 200) {
          *(int *)(current_player * 0xc + 0x1109) = unaff_DI;
          iVar4 = dice(1,0xc,10);
          *(int *)(current_player * 0xc + 0x110b) = iVar4;
          *(undefined2 *)(current_player * 0xc + 0x110d) = 0;
          return;
        }
      }
      goto LAB_4976_0db6;
    case 0x4aa02:
      if (game_won != 0) goto LAB_4976_0db6;
      bVar2 = false;
      for (iStack_c = 0; (!bVar2 && (iStack_c < 200)); iStack_c = iStack_c + 1) {
        if (param_1 == 0) {
          iVar4 = 0x3c;
        }
        else {
          iVar4 = 0x28;
        }
        unaff_DI = dice(1,city_count,-1);
        if (((*(char *)(unaff_DI * 0x41 + 0x1592) != current_player) &&
            (*(char *)(*(byte *)(*(int *)(unaff_DI * 0x41 + 0x157f) * 0xe0 +
                                *(int *)(unaff_DI * 0x41 + 0x157d) * 2) + 0x710) == '\n')) &&
           ((iVar6 = map_distance(*_DAT_451b_1efe,((int *)_DAT_451b_1efe)[1],
                                  *(int *)(unaff_DI * 0x41 + 0x157d),
                                  *(int *)(unaff_DI * 0x41 + 0x157f)), iVar6 <= iVar4 ||
            (99 < iStack_c)))) {
          bVar2 = true;
        }
      }
      break;
    case 0x4ab09:
      if (game_won != 0) goto LAB_4976_0db6;
      bVar2 = false;
      for (iStack_c = 0; (!bVar2 && (iStack_c < 200)); iStack_c = iStack_c + 1) {
        unaff_DI = dice(1,city_count,-1);
        if (((*(char *)(unaff_DI * 0x41 + 0x1592) != current_player) &&
            (*(char *)(*(byte *)(*(int *)(unaff_DI * 0x41 + 0x157f) * 0xe0 +
                                *(int *)(unaff_DI * 0x41 + 0x157d) * 2) + 0x710) == '\n')) &&
           ((iVar4 = map_distance(*_DAT_451b_1efe,((int *)_DAT_451b_1efe)[1],
                                  *(int *)(unaff_DI * 0x41 + 0x157d),
                                  *(int *)(unaff_DI * 0x41 + 0x157f)), iVar4 < 0x3d ||
            (99 < iStack_c)))) {
          bVar2 = true;
        }
      }
      break;
    case 0x4ac15:
      *(undefined2 *)(current_player * 0xc + 0x1109) = 0;
      *(undefined2 *)(current_player * 0xc + 0x110d) = 0;
      iVar4 = dice(3,300,500);
      *(int *)(current_player * 0xc + 0x110b) = iVar4;
      return;
    }
    if (iStack_c < 200) {
LAB_4976_149c:
      *(int *)(current_player * 0xc + 0x1109) = unaff_DI;
      return;
    }
  } while( true );
LAB_4976_1097:
  if (0x15 < iStack_6) {
LAB_4976_10a0:
    *(int *)(current_player * 0xc + 0x1109) = iStack_6;
    FUN_66d4_0b36(iStack_6,&iStack_8);
    FUN_8611_1298();
    iVar4 = FUN_828e_04b5(iStack_8,iStack_a);
    if (-1 < iVar4) {
      puVar1 = (uint *)(iVar4 * 0x1f + 0x82e);
      *puVar1 = *puVar1 | 1 << ((byte)current_player & 0x1f);
      FUN_834b_2785();
    }
    FUN_834b_2200();
    return;
  }
  if (param_1 == 0) {
    iVar6 = 0x32;
  }
  else {
    iVar6 = 0x1e;
  }
  FUN_66d4_0b36(iStack_6,&iStack_8);
  if (*(char *)(iStack_6 * 0x1d + 0xcff) != '\0') {
    __F_SPUSH();
    iVar5 = item_reserved();
    if (iVar5 == 0) {
      uVar7 = *_DAT_451b_1efe - iStack_8 >> 0xf;
      if ((((int)((*_DAT_451b_1efe - iStack_8 ^ uVar7) - uVar7) < iVar6) &&
          (uVar7 = ((int *)_DAT_451b_1efe)[1] - iStack_a, uVar8 = (int)uVar7 >> 0xf,
          (int)((uVar7 ^ uVar8) - uVar8) < iVar6)) &&
         (iStack_10 = iStack_10 + 1, iStack_10 == iVar4)) goto LAB_4976_10a0;
    }
  }
  iStack_6 = iStack_6 + 1;
  goto LAB_4976_1097;
  while (((iVar4 = iVar5, *(char *)(iVar5 * 0x16 + -0x777c) != '\x1c' ||
          (*(char *)(iVar5 * 0x16 + -0x777b) == current_player)) ||
         (iStack_10 = iStack_10 + 1, iStack_10 != iVar6))) {
LAB_4976_0f32:
    iVar5 = iVar4 + -1;
    if (iVar4 == 0) break;
  }
  *(int *)(current_player * 0xc + 0x1109) = iVar5;
  return;
}


