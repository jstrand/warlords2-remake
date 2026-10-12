
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_city_info(int param_1)

{
  undefined4 uVar1;
  undefined2 uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  int iVar8;
  undefined2 unaff_SI;
  int unaff_DI;
  undefined2 unaff_SS;
  char *pcVar9;
  undefined2 uVar10;
  char *pcVar11;
  undefined2 uVar12;
  undefined4 uVar13;
  undefined1 local_116 [120];
  char local_9e [50];
  char local_6c [50];
  int local_3a;
  int local_38;
  int local_36 [8];
  undefined1 local_26 [4];
  undefined1 local_22 [8];
  int local_1a;
  int local_18;
  int local_16;
  int local_14;
  int local_12 [5];
  undefined1 local_8 [2];
  int local_6;
  uint local_4;
  
  uVar13 = CONCAT22(unaff_SI,unaff_DI);
  FUN_22bf_07a5();
  if (param_1 != 0) {
    if (DAT_4125_166a == 0) {
      FUN_54f6_0000(2,uVar13);
    }
    else {
      FUN_54f6_07e4(0,0,0,0x32);
    }
  }
  if (4 < LAB_4125_0e92) goto LAB_7204_1a88;
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)(LAB_4125_0e92 * 2 + 0x1a91)) {
  case 0x72767:
    FUN_78a8_06ae(1,0xf,0,3);
    FUN_7ecb_00d6(0x1b0,0x3e,DAT_4125_0e98 * 0x41 + 0x1581,0x3c04);
    FUN_8611_0bf7((int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1592),0,0x342a,0x4125,(int)DAT_4125_0ea0,
                  (int)((ulong)DAT_4125_0ea0 >> 0x10));
    FUN_8611_0bf7((int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1592),0,0x342a,0x4125,(int)DAT_4125_0ea4,
                  (int)((ulong)DAT_4125_0ea4 >> 0x10));
    FUN_78a8_06ae(2,0xf,0,3);
    if (*(char *)(*(byte *)(*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(DAT_4125_0e98 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
      uVar7 = *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x15a7);
    }
    else {
      uVar7 = 0;
    }
    pcVar9 = get_string(0x74,0);
    sprintf((char *)CONCAT22(unaff_SS,local_6c),pcVar9,uVar7);
    FUN_7ecb_00bf((int)DAT_4125_0ea8,(int)((ulong)DAT_4125_0ea8 >> 0x10),local_6c);
    if (*(char *)(*(byte *)(*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(DAT_4125_0e98 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
      iVar8 = (int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1591);
    }
    else {
      iVar8 = 0;
    }
    pcVar9 = get_string(0x74,1);
    sprintf((char *)CONCAT22(unaff_SS,local_6c),pcVar9,iVar8);
    FUN_7ecb_00bf((int)DAT_4125_0eac,(int)((ulong)DAT_4125_0eac >> 0x10),local_6c);
    if (*(char *)(*(byte *)(*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(DAT_4125_0e98 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
      if (*(char *)(DAT_4125_0e98 * 0x41 + 0x1592) == '\x0f') {
        iVar8 = 3;
        goto LAB_7204_090f;
      }
      iVar8 = *(char *)(DAT_4125_0e98 * 0x41 + 0x1592) * 0x14;
      uVar2 = 0x3c04;
      pcVar9 = get_string(0x74,4);
      uVar7 = (undefined2)((ulong)pcVar9 >> 0x10);
      sprintf((char *)CONCAT22(unaff_SS,local_6c),pcVar9,iVar8,uVar2);
    }
    else {
      iVar8 = 2;
LAB_7204_090f:
      pcVar9 = get_string(0x74,iVar8);
      pcVar9 = strcpy((char *)CONCAT22(unaff_SS,local_6c),pcVar9);
      uVar7 = (undefined2)((ulong)pcVar9 >> 0x10);
    }
    FUN_7ecb_00bf((int)DAT_4125_0eb0,(int)((ulong)DAT_4125_0eb0 >> 0x10),local_6c);
    if ((*(char *)(*(byte *)(*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) * 0xe0 +
                            *(int *)(DAT_4125_0e98 * 0x41 + 0x157d) * 2) + 0x710) == '\n') &&
       ((opt_view_production == 0 ||
        (uVar7 = 0x3c04, *(char *)(DAT_4125_0e98 * 0x41 + 0x1592) == current_player)))) {
      iVar4 = 0;
      for (iVar8 = 0; iVar8 < 4; iVar8 = iVar8 + 1) {
        FUN_8611_08be((int)*(char *)(DAT_4125_0e98 * 0x41 + iVar8 + 0x1593),
                      (int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1592),0x342a,0x4125,
                      *(undefined2 *)(iVar4 * 4 + 0xf18),*(undefined2 *)(iVar4 * 4 + 0xf1a),0,0,0,0,
                      1,0,0);
        iVar4 = iVar4 + 1;
      }
    }
    else {
      FUN_24d0_0002(0);
      uVar2 = FUN_216d_002e(local_22);
      FUN_216d_01fd(uVar2,uVar7);
      uVar10 = 4;
      uVar12 = 2;
      uVar2 = FUN_216d_002e(local_22);
      FUN_24d0_02e5(uVar2,uVar7,uVar10,uVar12);
      FUN_216d_002e(local_22);
      FUN_216d_000e(local_26);
      __F_SPUSH(local_26._0_2_,local_26._2_2_);
      FUN_1997_0129(0x37);
      FUN_8611_0860(*(undefined1 *)
                     (*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) * 0xe0 +
                     *(int *)(DAT_4125_0e98 * 0x41 + 0x157d) * 2),0x342a,0x4125,0xb00188);
      FUN_8611_0860(*(undefined1 *)
                     (*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) * 0xe0 +
                      *(int *)(DAT_4125_0e98 * 0x41 + 0x157d) * 2 + 0xe0),0x342a,0x4125,0xd80188);
      FUN_8611_0860(*(undefined1 *)
                     (*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) * 0xe0 +
                      *(int *)(DAT_4125_0e98 * 0x41 + 0x157d) * 2 + 2),0x342a,0x4125,0xb001b0);
      FUN_8611_0860(*(undefined1 *)
                     (*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) * 0xe0 +
                      *(int *)(DAT_4125_0e98 * 0x41 + 0x157d) * 2 + 0xe2),0x342a,0x4125,0xd801b0);
    }
    for (iVar8 = 0; iVar8 < 8; iVar8 = iVar8 + 1) {
      if ((*(int *)(DAT_4125_0e98 * 0x41 + 0x157d) == *(int *)(iVar8 * 0x14 + 0x193)) &&
         (*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) == *(int *)(iVar8 * 0x14 + 0x195))) {
        FUN_8611_0bf7(iVar8,4,0x342a,0x4125,0x198,0xaa);
        break;
      }
    }
    FUN_7204_1f06(1,DAT_4125_0e98,local_116);
    for (iVar8 = 0; iVar8 < 3; iVar8 = iVar8 + 1) {
      FUN_7ecb_00bf(*(undefined2 *)(iVar8 * 4 + 0xec8),*(undefined2 *)(iVar8 * 4 + 0xeca),
                    local_116 + iVar8 * 0x28);
    }
    break;
  case 0x72d1f:
    FUN_78a8_06ae(1,0xf,0,3);
    FUN_7ecb_00d6(0x1b0,0x3e,DAT_4125_0e98 * 0x1f + 0x815,0x3c04);
    FUN_24d0_02e5(0xedc,0x4125,4,2);
    FUN_24d0_0002(0);
    FUN_216d_01fd(0xed4,0x4125);
    if (*(char *)(DAT_4125_0e98 * 0x1f + 0x829) == '\x01') {
      local_6 = 0;
    }
    else {
      local_6 = (int)DAT_4125_0e98 % 5 + 1;
    }
    __F_SPUSH((int)_LAB_4125_0ee3_1,(int)((ulong)_LAB_4125_0ee3_1 >> 0x10));
    FUN_1997_0129(6);
    FUN_78a8_06ae(0xf0002,0x30000);
    pcVar9 = get_string(0x71,(int)*(char *)(DAT_4125_0e98 * 0x1f + 0x829));
    sprintf((char *)CONCAT22(unaff_SS,local_6c),pcVar9);
    FUN_7ecb_00bf(DAT_4125_0eb4,local_6c);
    local_4 = (uint)((*(byte *)(DAT_4125_0e9c * 0xe0 + DAT_4125_0e9a * 2 + 1) & 0x40) != 0);
    pcVar9 = get_string(0x72,local_4);
    sprintf((char *)CONCAT22(unaff_SS,local_6c),pcVar9);
    FUN_7ecb_00bf(DAT_4125_0eb8,local_6c);
    FUN_24d0_0002();
    FUN_216d_0288(0xf50);
    FUN_24d0_0002();
    uVar13 = 0x24d02e8a;
    FUN_216d_01fd(0xf50);
    for (iVar8 = 0; iVar8 < 4; iVar8 = iVar8 + 1) {
      uVar1 = *(undefined4 *)(iVar8 * 4 + 0xf58);
      __F_SPUSH((int)uVar1,(int)((ulong)uVar1 >> 0x10),uVar13);
      FUN_1997_027e(9);
      pcVar9 = get_string(0x73,iVar8);
      uVar7 = *(undefined2 *)(iVar8 * 4 + 0xf6a);
      uVar2 = *(undefined2 *)(iVar8 * 4 + 0xf68);
      uVar13 = CONCAT22(uVar7,uVar2);
      FUN_7ecb_00bf(uVar2,uVar7,(char *)pcVar9);
    }
    FUN_7204_1f06(2,DAT_4125_0e98,local_116);
    for (iVar8 = 0; iVar8 < 3; iVar8 = iVar8 + 1) {
      FUN_7ecb_00bf(*(undefined2 *)(iVar8 * 4 + 0xec8),*(undefined2 *)(iVar8 * 4 + 0xeca),
                    local_116 + iVar8 * 0x28);
    }
    break;
  case 0x72f38:
    FUN_78a8_06ae(1,0xf,0,3);
    FUN_7ecb_00d6(0x1b0,0x3e,DAT_4125_0e98 * 0x41 + 0x1581,0x3c04);
    FUN_78a8_06ae(2,0xf,0,3);
    local_1a = 0;
    if (*(int *)((undefined1 *)&LAB_4125_15ad_1 + DAT_4125_0e98 * 0x41) != 0) {
      local_1a = 1;
      unaff_DI = FUN_828e_044a(*(undefined2 *)((undefined1 *)&LAB_4125_15b0 + DAT_4125_0e98 * 0x41),
                               *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x15b2));
    }
    for (iVar8 = 0; iVar8 < 8; iVar8 = iVar8 + 1) {
      if ((*(int *)(DAT_4125_0e98 * 0x41 + 0x157d) == *(int *)(iVar8 * 0x14 + 0x193)) &&
         (*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) == *(int *)(iVar8 * 0x14 + 0x195))) {
        FUN_8611_0bf7(iVar8,4,0x342a,0x4125,0x138,0x6e);
        break;
      }
    }
    pcVar9 = get_string(0x75,0);
    FUN_7ecb_0103(_LAB_4125_0f89_3 + 8,_LAB_4125_0f8d_1,pcVar9);
    if (*(char *)(DAT_4125_0e98 * 0x41 + 0x15aa) < '\x01') {
      iVar8 = -1;
      strcpy((char *)CONCAT22(unaff_SS,local_6c),(char *)0x41250fc8);
    }
    else {
      iVar8 = (int)*(char *)(DAT_4125_0e98 * 0x41 + 0x15a9);
      sprintf((char *)CONCAT22(unaff_SS,local_6c),(char *)0x41250fc4,
              (int)*(char *)(DAT_4125_0e98 * 0x41 + 0x15aa));
    }
    if (local_1a != 0) {
      pcVar9 = get_string(0x75,1);
      strcat((char *)CONCAT22(unaff_SS,local_6c),pcVar9);
      if (*(int *)((undefined1 *)&LAB_4125_15b0 + DAT_4125_0e98 * 0x41) == -2) {
        local_18 = FUN_7563_0b47(current_player,local_8);
        if (local_18 == 0) {
          iVar4 = 3;
        }
        else {
          iVar4 = 2;
        }
        pcVar9 = get_string(0x75,iVar4);
      }
      else {
        pcVar9 = (char *)CONCAT22(0x3c04,(char *)(unaff_DI * 0x41 + 0x1581));
      }
      strcpy((char *)CONCAT22(unaff_SS,local_9e),pcVar9);
    }
    FUN_8611_08be(iVar8,(int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1592),0x342a,0x4125,
                  (int)_LAB_4125_0f90,(int)((ulong)_LAB_4125_0f90 >> 0x10),0,0,0,0,1,0,0);
    if (local_1a == 0) {
      pcVar11 = local_6c;
      uVar7 = (undefined2)_LAB_4125_0f93_1;
      uVar2 = (undefined2)((ulong)_LAB_4125_0f93_1 >> 0x10);
    }
    else {
      FUN_7ecb_00bf((int)_LAB_4125_0f98,(int)((ulong)_LAB_4125_0f98 >> 0x10),local_6c);
      pcVar11 = local_9e;
      uVar7 = (undefined2)_LAB_4125_0f9c;
      uVar2 = (undefined2)((ulong)_LAB_4125_0f9c >> 0x10);
    }
    FUN_7ecb_00bf(uVar7,uVar2,pcVar11);
    iVar4 = 0;
    for (iVar8 = 0; iVar8 < 4; iVar8 = iVar8 + 1) {
      iVar5 = (int)*(char *)(DAT_4125_0e98 * 0x41 + iVar8 + 0x1593);
      if (-1 < iVar5) {
        if (DAT_4125_0e9e == iVar5) {
          iVar6 = current_player + 2;
          local_16 = iVar8;
        }
        else {
          iVar6 = 1;
        }
        FUN_8611_08be(iVar5,(int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1592),0x342a,0x4125,
                      *(undefined2 *)(iVar4 * 4 + 0xf78),*(undefined2 *)(iVar4 * 4 + 0xf7a),
                      (int)*(char *)(DAT_4125_0e98 * 0x41 + iVar8 + 0x1597),
                      (int)*(char *)(DAT_4125_0e98 * 0x41 + iVar8 + 0x15a3),
                      (int)*(char *)(DAT_4125_0e98 * 0x41 + iVar8 + 0x159f),
                      (int)*(char *)(DAT_4125_0e98 * 0x41 + iVar8 + 0x159b),iVar6,0,0);
        iVar4 = iVar4 + 1;
      }
    }
    if (-1 < DAT_4125_0e9e) {
      FUN_54e0_0000(0x342a,0x4125,(int)_LAB_4125_0f85_3,(int)((ulong)_LAB_4125_0f85_3 >> 0x10),
                    DAT_4125_0e9e,1,(int)*(char *)(DAT_4125_0e98 * 0x41 + local_16 + 0x1597),
                    (int)*(char *)(DAT_4125_0e98 * 0x41 + local_16 + 0x15a3),
                    (int)*(char *)(DAT_4125_0e98 * 0x41 + local_16 + 0x159b),
                    (int)*(char *)(DAT_4125_0e98 * 0x41 + local_16 + 0x159f));
    }
    break;
  case 0x732f7:
    FUN_78a8_06ae(1,0xf,0,3);
    FUN_7ecb_00d6(0x1b0,0x3e,DAT_4125_0e98 * 0x41 + 0x1581,0x3c04);
    FUN_78a8_06ae(2,0xf,0,3);
    iVar8 = -1;
    iVar4 = -1;
    if (*(int *)((undefined1 *)&LAB_4125_15ad_1 + DAT_4125_0e98 * 0x41) != 0) {
      iVar8 = -1;
      uVar3 = FUN_828e_044a(*(undefined2 *)((undefined1 *)&LAB_4125_15b0 + DAT_4125_0e98 * 0x41),
                            *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x15b2),uVar13,0xffff,0xffff);
      iVar5 = army_count;
      if ((int)uVar3 < 0) {
        FUN_7563_0b47(current_player,local_8);
        iVar5 = army_count;
      }
      while (iVar6 = iVar5 + -1, iVar5 != 0) {
        iVar5 = iVar6;
        if (((int)*(char *)(iVar6 * 0x16 + -0x7777) == DAT_4125_0e98) &&
           ((((*(int *)(iVar6 * 0x16 + -0x7780) == -1 &&
              (*(byte *)(iVar6 * 0x16 + -0x7771) >> 1 == uVar3)) ||
             (*(int *)(iVar6 * 0x16 + -0x7780) == -2)) &&
            (*(char *)(iVar6 * 0x16 + -0x777b) == current_player)))) {
          if (*(char *)(iVar6 * 0x16 + -0x7770) == 'e') {
            iVar4 = (int)*(char *)(iVar6 * 0x16 + -0x777c);
          }
          else {
            iVar8 = (int)*(char *)(iVar6 * 0x16 + -0x777c);
          }
        }
      }
    }
    pcVar9 = get_string(0x9a,0);
    FUN_7ecb_0103(0x168,0x6d,pcVar9);
    if (*(char *)(DAT_4125_0e98 * 0x41 + 0x15aa) < '\x01') {
      iVar5 = -1;
      strcpy((char *)CONCAT22(unaff_SS,local_6c),(char *)0x41250fca);
    }
    else {
      iVar5 = (int)*(char *)(DAT_4125_0e98 * 0x41 + 0x15a9);
      iVar6 = (int)*(char *)(DAT_4125_0e98 * 0x41 + 0x15aa);
      pcVar9 = get_string(0x9a,1);
      sprintf((char *)CONCAT22(unaff_SS,local_6c),pcVar9,iVar6);
    }
    FUN_8611_08be(iVar5,(int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1592),0x342a,0x4125,0x170,0x67,0,0,0
                  ,0,1,0,0);
    FUN_7ecb_00bf(0x198,0x6d,local_6c);
    FUN_8611_08be(iVar4,(int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1592),0x342a,0x4125,0x1b0,0x67,0,0,0
                  ,0,1,0,0);
    FUN_8611_08be(iVar8,(int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1592),0x342a,0x4125,0x1d8,0x67,0,0,0
                  ,0,1,0,0);
    FUN_7087_0410(DAT_4125_0e98,local_12);
    local_3a = 0;
    local_38 = 0;
    local_36[7] = 0xffff;
    local_36[6] = 0xffff;
    local_36[5] = 0xffff;
    local_36[4] = 0xffff;
    local_36[3] = 0xffff;
    local_36[2] = 0xffff;
    local_36[1] = 0xffff;
    local_36[0] = -1;
    for (iVar8 = 0; iVar8 < local_14; iVar8 = iVar8 + 1) {
      iVar4 = local_12[iVar8];
      iVar5 = army_count;
      while (((iVar6 = iVar5 + -1, iVar5 != 0 && (local_38 < 4)) && (local_3a < 4))) {
        iVar5 = iVar6;
        if (((*(char *)(iVar6 * 0x16 + -0x7777) == iVar4) &&
            (*(int *)(iVar6 * 0x16 + -0x7780) == -1)) &&
           ((*(byte *)(iVar6 * 0x16 + -0x7771) >> 1 == DAT_4125_0e98 &&
            (*(char *)(iVar6 * 0x16 + -0x777b) == current_player)))) {
          if (*(char *)(iVar6 * 0x16 + -0x7770) == 'e') {
            local_36[local_3a] = (int)*(char *)(iVar6 * 0x16 + -0x777c);
            local_3a = local_3a + 1;
          }
          else {
            local_36[local_38 + 4] = (int)*(char *)(iVar6 * 0x16 + -0x777c);
            local_38 = local_38 + 1;
          }
        }
      }
    }
    pcVar9 = get_string(0x9a,2);
    FUN_7ecb_0103(0x188,0x9b,pcVar9);
    pcVar9 = get_string(0x9a,3);
    FUN_7ecb_0103(0x188,0xbc,pcVar9);
    for (iVar8 = 0; iVar8 < 4; iVar8 = iVar8 + 1) {
      FUN_8611_08be(local_36[iVar8 + 4],current_player,0x342a,0x4125,iVar8 * 0x28 + 400,0x95,0,0,0,0
                    ,1,0,0);
      FUN_8611_08be(local_36[iVar8],current_player,0x342a,0x4125,iVar8 * 0x28 + 400,0xb6,0,0,0,0,1,0
                    ,0);
    }
    auto_ui_vector_help(0);
    auto_ui_vector_help(1);
    break;
  case 0x737a9:
    FUN_78a8_06ae(1,0xf,0,3);
    FUN_7ecb_00d6(0x1b0,0x3e,DAT_4125_0e98 * 0x41 + 0x1581,0x3c04);
    FUN_8611_0bf7((int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1592),0,0x342a,0x4125,(int)DAT_4125_0ea0,
                  (int)((ulong)DAT_4125_0ea0 >> 0x10));
    FUN_8611_0bf7((int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1592),0,0x342a,0x4125,(int)DAT_4125_0ea4,
                  (int)((ulong)DAT_4125_0ea4 >> 0x10));
    FUN_78a8_06ae(2,0xf,0,3);
    if (*(char *)(*(byte *)(*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(DAT_4125_0e98 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
      uVar7 = *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x15a7);
    }
    else {
      uVar7 = 0;
    }
    pcVar9 = get_string(0x74,0);
    sprintf((char *)CONCAT22(unaff_SS,local_6c),pcVar9,uVar7);
    FUN_7ecb_00bf((int)DAT_4125_0ea8,(int)((ulong)DAT_4125_0ea8 >> 0x10),local_6c);
    if (*(char *)(*(byte *)(*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(DAT_4125_0e98 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
      iVar8 = (int)*(char *)(DAT_4125_0e98 * 0x41 + 0x1591);
    }
    else {
      iVar8 = 0;
    }
    pcVar9 = get_string(0x74,1);
    sprintf((char *)CONCAT22(unaff_SS,local_6c),pcVar9,iVar8);
    FUN_7ecb_00bf((int)DAT_4125_0eac,(int)((ulong)DAT_4125_0eac >> 0x10),local_6c);
    if (*(char *)(*(byte *)(*(int *)(DAT_4125_0e98 * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(DAT_4125_0e98 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
      if (*(char *)(DAT_4125_0e98 * 0x41 + 0x1592) == '\x0f') {
        iVar8 = 3;
        goto LAB_7204_1951;
      }
      iVar8 = *(char *)(DAT_4125_0e98 * 0x41 + 0x1592) * 0x14;
      uVar7 = 0x3c04;
      pcVar9 = get_string(0x74,4);
      sprintf((char *)CONCAT22(unaff_SS,local_6c),pcVar9,iVar8,uVar7);
    }
    else {
      iVar8 = 2;
LAB_7204_1951:
      pcVar9 = get_string(0x74,iVar8);
      strcpy((char *)CONCAT22(unaff_SS,local_6c),pcVar9);
    }
    FUN_7ecb_00bf((int)DAT_4125_0eb0,(int)((ulong)DAT_4125_0eb0 >> 0x10),local_6c);
    FUN_78a8_06ae(2,0xf,0,3);
    pcVar9 = get_string(0x77,0);
    FUN_7ecb_00bf((int)_LAB_4125_0fa8,(int)((ulong)_LAB_4125_0fa8 >> 0x10),pcVar9);
    pcVar9 = get_string(0x77,1);
    FUN_7ecb_00bf((int)_LAB_4125_0fac,(int)((ulong)_LAB_4125_0fac >> 0x10),pcVar9);
    pcVar9 = get_string(0x78,0);
    FUN_7ecb_00bf((int)_LAB_4125_0faf_1,(int)((ulong)_LAB_4125_0faf_1 >> 0x10),pcVar9);
    pcVar9 = get_string(0x78,1);
    FUN_7ecb_00bf((int)_LAB_4125_0fb4,(int)((ulong)_LAB_4125_0fb4 >> 0x10),pcVar9);
    pcVar9 = get_string(0x79,0);
    FUN_7ecb_00bf((int)_LAB_4125_0fb7_1,(int)((ulong)_LAB_4125_0fb7_1 >> 0x10),pcVar9);
    pcVar9 = get_string(0x79,1);
    FUN_7ecb_00bf((int)_LAB_4125_0fbb_1,(int)((ulong)_LAB_4125_0fbb_1 >> 0x10),pcVar9);
  }
LAB_7204_1a88:
  FUN_22bf_0757();
  return;
}


