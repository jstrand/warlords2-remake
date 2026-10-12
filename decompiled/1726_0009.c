
/* WARNING: Instruction at (ram,0x0001e58c) overlaps instruction at (ram,0x0001e58b)
    */
/* WARNING (jumptable): Unable to track spacebase fully for stack */
/* WARNING: Unable to track spacebase fully for stack */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

code * __cdecl16far FUN_1726_0009(code *param_1,code *param_2,int *param_3)

{
  byte *pbVar1;
  char *pcVar2;
  undefined2 *puVar3;
  code **ppcVar4;
  code cVar5;
  code *pcVar6;
  int *piVar7;
  byte *pbVar8;
  code *pcVar9;
  char cVar10;
  int iVar11;
  byte *pbVar12;
  code *pcVar13;
  byte bVar16;
  int iVar14;
  int in_CX;
  uint uVar15;
  undefined2 extraout_DX;
  undefined2 extraout_DX_00;
  int iVar17;
  code *pcVar18;
  code *extraout_DX_01;
  code *extraout_DX_02;
  undefined2 extraout_DX_03;
  int iVar19;
  byte *pbVar20;
  uint uVar21;
  code *pcVar22;
  code *pcVar23;
  int iVar24;
  int iVar25;
  code *pcVar26;
  undefined1 *puVar27;
  undefined2 *puVar28;
  code **ppcVar29;
  byte *pbVar30;
  byte *unaff_DI;
  undefined2 uVar31;
  int *piVar32;
  code *pcVar33;
  undefined2 uVar34;
  char *pcVar35;
  int unaff_SS;
  bool bVar36;
  bool bVar37;
  char cVar38;
  bool bVar39;
  undefined4 uVar40;
  code *in_stack_00000002;
  code *pcStack_2c;
  code *pcStack_2a;
  code *pcStack_28;
  undefined2 *puStack_16;
  int local_14;
  byte *pbStack_12;
  undefined2 *local_10;
  int local_e;
  byte *pbStack_c;
  undefined2 *puStack_a;
  undefined2 uStack_8;
  undefined4 local_6;
  
  pcVar35 = (char *)s_Join_file_not_found_4125_1720 + 6;
  if (DAT_4125_166c != 0) {
    return (code *)0x0;
  }
  if ((code *)param_1 == (code *)0x0) {
    pcVar35 = (char *)0x22bf;
    pbVar12 = (byte *)FUN_22bf_0944();
    if (pbVar12 == (byte *)0x0) {
      if (_LAB_4125_1174 != 0) {
        _LAB_4125_1174 = 0;
        FUN_740d_11cf();
      }
      if (_LAB_4125_1176 != 0) {
        _LAB_4125_1176 = 0;
      }
      if (_LAB_4125_1172 != 0) {
        _LAB_4125_1172 = 0;
        iVar19 = _LAB_4125_1178 + 1;
        bVar36 = 6 < _LAB_4125_1178;
        _LAB_4125_1178 = iVar19;
        if (bVar36) {
          FUN_8065_0b3b();
          _LAB_4125_1178 = 7;
        }
      }
      _LAB_4125_117a = 0;
      return (code *)0x0;
    }
  }
  else {
    _LAB_4125_117a = 0;
    pbVar12 = unaff_DI;
  }
  if (_LAB_4125_1174 != 0) {
    return (code *)0x0;
  }
  if (_LAB_4125_1172 != 0) {
    if (_LAB_4125_1178 < 7) {
      _LAB_4125_1178 = _LAB_4125_1178 + 1;
      return (code *)0x0;
    }
    _LAB_4125_1178 = _LAB_4125_1178 + 1;
    FUN_22bf_0af4();
    FUN_8065_0b3b();
    _LAB_4125_1178 = 7;
    return (code *)0x0;
  }
  if (_LAB_4125_1176 != 0) {
    FUN_22bf_0af4();
    FUN_8065_0e04();
    return (code *)0x0;
  }
  pcVar26 = *(code **)*(undefined4 *)((int)DAT_451b_0ce0 + 0xc);
  do {
    do {
      do {
        pcVar23 = pcVar26;
        pcVar26 = pcVar23 + -1;
        if (pcVar23 == (code *)0x0) {
          return (code *)0x0;
        }
        uVar31 = (undefined2)((ulong)DAT_451b_0ce0 >> 0x10);
        uVar34 = *(undefined2 *)((int)DAT_451b_0ce0 + 6);
        pcVar18 = (code *)(*(int *)((int)DAT_451b_0ce0 + 4) + (int)pcVar26 * 0xe);
        local_6 = (code *)CONCAT22(uVar34,pcVar18);
      } while (*(int *)(pcVar18 + 2) != 1);
      pcStack_28 = (code *)&local_e;
      pcStack_2c = (code *)0x73a2;
      pcStack_2a = (code *)pcVar35;
      FUN_216d_002e();
      if (*(int *)((code *)local_6 + 0xc) == 0) {
        FUN_24b0_0137();
      }
      pcVar35 = (char *)0x22bf;
      pcVar13 = (code *)FUN_22bf_08b1();
    } while (pcVar13 == (code *)0x0);
    piVar32 = (int *)((ulong)local_6 >> 0x10);
    uVar21 = *(int *)local_6 - 1;
  } while (0xe < uVar21);
  pcVar22 = (code *)(uVar21 * 2);
  bVar39 = (int)uVar21 < 0 != (int)pcVar22 < 0;
  bVar37 = (int)pcVar22 < 0;
  bVar36 = pcVar22 == (code *)0x0;
  pcVar33 = (code *)((ulong)param_1 >> 0x10);
  pcVar9 = DAT_4125_337e;
  switch(uVar21) {
  case 0:
    iVar19 = uVar21 * 4;
    goto switchD_1000_2ec3_switchD;
  case 1:
    if (bVar36 || bVar39 != bVar37) {
      if (bVar37) {
        pcVar6 = (code *)swi(0x33);
        pcStack_2a = pcVar18;
        (*pcVar6)();
        DAT_4125_3560 = in_CX >> ((byte)DAT_451b_2a56 & 0x1f);
        DAT_4125_3562 = extraout_DX_03;
        return pcVar22;
      }
      goto LAB_1ea3_01a7;
    }
    break;
  case 2:
    if (!bVar36) {
      if ((code *)param_1 == (code *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 6))
      break;
      if ((code *)param_1 == (code *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 6)) {
LAB_1ea3_0193:
        DAT_4125_338a = 1;
        pcVar13 = (code *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 6);
      }
      else {
        pcVar6 = (code *)swi(0x10);
        cVar10 = (*pcVar6)();
        if (cVar10 == '\x1a') {
          cVar10 = (char)pcVar22;
          pcVar26 = extraout_DX_01;
          pcVar23 = extraout_DX_01;
          if ((cVar10 != '\a') && (pcVar23 = extraout_DX_01, cVar10 != '\b')) {
            if ((cVar10 == '\n') || ((cVar10 == '\v' || (cVar10 == '\f')))) {
              DAT_4125_3382 = extraout_DX_01;
              DAT_4125_3386 = extraout_DX_01;
              return (code *)((int)(undefined2 *)&DAT_4125_0002 + 1);
            }
            goto LAB_1ea3_0124;
          }
        }
        else {
LAB_1ea3_0124:
          iVar19 = 0;
          pcVar6 = (code *)swi(0x10);
          (*pcVar6)();
          pcVar26 = extraout_DX_02;
          pcVar23 = DAT_4125_3388;
          if (iVar19 == 0) {
            bVar36 = (byte)_DAT_f000_c000 < 0x21;
            pcVar18 = extraout_DX_02;
            if ((byte)_DAT_f000_c000 == 0x21) goto LAB_1ea3_0150;
            FUN_1ea3_01b8();
            pcVar9 = DAT_4125_337e;
            if (!bVar36) break;
            goto LAB_1ea3_0193;
          }
        }
        DAT_4125_3388 = pcVar23;
        pcVar13 = (code *)0x0;
        DAT_4125_3382 = pcVar26;
        DAT_4125_3384 = pcVar26;
      }
      goto LAB_1ea3_01a7;
    }
LAB_1ea3_0150:
    DAT_4125_337c = pcVar18;
    DAT_4125_3378 = DAT_4125_3378 + -1;
    if (DAT_4125_3376 < 0xf1) {
      DAT_4125_337a = pcVar18;
    }
    pcVar9 = pcVar18;
    if ((0x16f < DAT_4125_3376) &&
       ((code *)param_1 != (code *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 6))) {
      DAT_4125_3382 = pcVar18;
      DAT_4125_3380 = pcVar18;
      return (code *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 6);
    }
    break;
  case 3:
    do {
      iVar19 = in_CX;
      if (*(byte **)(pcVar22 + -0x777e) == pbVar12) {
        uStack_8 = uStack_8 + 1;
        local_6 = (code *)(ulong)(in_CX * 0x16 + 0x8880);
      }
      do {
        in_CX = iVar19 + -1;
        if (iVar19 == 0) {
          if (uStack_8 == 0) {
            pbVar1 = (byte *)((int)pbVar12 * 0xe0 + (int)pcVar26 * 2 + 1);
            *pbVar1 = *pbVar1 & 0xef;
            pbVar1 = (byte *)((int)pbVar12 * 0xe0 + (int)pcVar26 * 2 + 1);
            *pbVar1 = *pbVar1 & 0xdf;
          }
          return (code *)local_6;
        }
        iVar19 = in_CX;
      } while ((code *)*(undefined2 *)(in_CX * 0x16 + -0x7780) != pcVar26);
      pcVar22 = (code *)(in_CX * 0x16);
    } while( true );
  case 4:
    pcRam000414e1 = pcVar18;
  case 0xb:
    pcVar6 = (code *)swi(0x21);
    pcRam00010291 = pcVar18;
    uVar40 = (*pcVar6)();
    uVar34 = s_Borland_C_____Copyright_1991_Bor_4125_0004._40_2_;
    iVar19 = DAT_4125_0002;
    uVar31 = (undefined2)((ulong)uVar40 >> 0x10);
    *(undefined2 *)&DAT_4125_007d = (int)uVar40;
    *(undefined2 *)&DAT_4125_007b = piVar32;
    *(undefined2 *)(char *)&DAT_4125_0077 = uVar34;
    *(int *)&DAT_4125_0091 = iVar19;
    __save_vectors();
    pcVar26 = (code *)0x0;
    iVar11 = 0;
    pcVar35 = (char *)0x0;
    uVar21 = 0x7fff;
LAB_1000_0037:
    do {
      if (uVar21 != 0) {
        uVar21 = uVar21 - 1;
        pcVar2 = pcVar35;
        pcVar35 = pcVar35 + 1;
        if (*pcVar2 != '\0') goto LAB_1000_0037;
      }
      if (uVar21 == 0) goto LAB_1000_007e;
      iVar11 = iVar11 + 1;
    } while (*pcVar35 != '\0');
    *(int *)&DAT_4125_0075 = -(uVar21 | 0x8000);
    *(uint *)&DAT_4125_0079 = iVar11 * 4 + 0x10U & 0xfff0;
    if (DAT_4125_26ca < 0x200) {
      DAT_4125_26ca = (undefined1 *)0x200;
    }
    bVar16 = 4;
    uVar21 = ((uint)DAT_4125_26ca >> 4) + 1;
    if ((uint)(iVar19 - unaff_SS) < uVar21) {
LAB_1000_007e:
      abort();
      return pcVar26;
    }
    iVar19 = uVar21 + unaff_SS;
    *(int *)&DAT_4125_0089 = iVar19;
    *(int *)&DAT_4125_008d = iVar19;
    pcVar6 = (code *)swi(0x21);
    (*pcVar6)();
    pcVar26 = pcRam00010291;
    puVar27 = (undefined1 *)((int)puStack_16 << (bVar16 & 0x1f));
    puVar28 = (undefined2 *)&DAT_451b_0000;
    DAT_4125_26ca = puVar27;
    for (pcVar35 = (char *)s_Your_Upkeep_4125_3201 + 5; pcVar35 != (char *)0x0;
        pcVar35 = pcVar35 + -1) {
      puVar3 = puVar28;
      puVar28 = (undefined2 *)((int)puVar28 + 1);
      *(undefined1 *)puVar3 = 0;
    }
    if (0x14 < *(uint *)&DAT_4125_3cfc) {
      bVar36 = *(byte *)&DAT_4125_007d < 3;
      if ((!bVar36) &&
         ((3 < *(byte *)&DAT_4125_007d || (bVar36 = *(byte *)&DAT_4125_007e < 0x1e, !bVar36)))) {
        pcVar6 = (code *)swi(0x21);
        pcVar26 = (code *)(*pcVar6)();
        if (!bVar36) {
          pcVar6 = (code *)swi(0x21);
          pcVar26 = (code *)(*pcVar6)();
          if (!bVar36) {
            pcVar6 = (code *)swi(0x21);
            pcVar26 = (code *)(*pcVar6)();
            if (!bVar36) {
              *(int *)&DAT_4125_0091 = (int)(pcVar26 + 1);
              pcVar6 = (code *)swi(0x21);
              pcVar26 = (code *)(*pcVar6)();
              if (!bVar36) {
                pcVar6 = (code *)swi(0x21);
                pcVar26 = (code *)(*pcVar6)();
                if (!bVar36) goto LAB_1000_010c;
              }
            }
          }
        }
        abort();
        return pcVar26;
      }
    }
LAB_1000_010c:
    pcVar6 = (code *)swi(0x1a);
    cVar10 = (*pcVar6)();
    *(undefined2 *)&DAT_4125_0081 = extraout_DX_00;
    *(undefined2 *)&DAT_4125_0083 = pcVar35;
    if (cVar10 != '\0') {
      DAT_0000_0470 = 1;
    }
    *(undefined2 *)(puVar27 + -2) = 0x138;
    __run_startup_procs();
    *(undefined2 *)(puVar27 + -2) = *(undefined2 *)&DAT_4125_0073;
    *(undefined2 *)(puVar27 + -4) = *(undefined2 *)&DAT_4125_0071;
    *(undefined2 *)(puVar27 + -6) = *(undefined2 *)&DAT_4125_006f;
    *(undefined2 *)(puVar27 + -8) = *(undefined2 *)&DAT_4125_006d;
    *(undefined2 *)(puVar27 + -10) = *(undefined2 *)&DAT_4125_006b;
    *(undefined2 *)(puVar27 + -0xc) = 0x1000;
    *(undefined2 *)(puVar27 + -0xe) = 0x151;
    uVar34 = main();
    *(undefined2 *)(puVar27 + -0xc) = uVar34;
    *(undefined2 *)(puVar27 + -0xe) = 0x1a4c;
    *(undefined2 *)(puVar27 + -0x10) = 0x157;
    exit(*(int *)(puVar27 + -0xc));
    *(undefined2 *)(puVar27 + -0xe) = 0x3f2a;
    *(undefined2 *)(puVar27 + -0x10) = 0x3f54;
    *(undefined2 *)(puVar27 + -0x12) = 0x167;
    pcVar26 = (code *)__run_exit_procs();
    return pcVar26;
  case 5:
    pcVar6 = pcVar23 + 0x1b;
    *pcVar6 = (code)((byte)*pcVar6 ^ 0x8e);
    if (-1 < (char)*pcVar6) {
      if ((*pbVar12 & 7) == 0) {
        pcStack_28 = (code *)0x5e7b;
        path_mark_cities();
      }
      if (*(int *)((code *)param_1 + 10) != 1) {
        if (*(int *)((code *)param_1 + 10) == 0) {
          pbStack_c = (byte *)&DAT_451b_0550;
          local_e = -0x7780;
          local_10 = (undefined2 *)&DAT_451b_04e0;
          iVar19 = 0x70;
          while( true ) {
            iVar11 = (int)path_cost_grid;
            pbStack_c = pbStack_c + -1;
            local_e = local_e + -0x138;
            local_10 = local_10 + -0x4e;
            iVar17 = iVar19 + -1;
            if (iVar19 == 0) break;
            iVar14 = 0x138;
            iVar24 = 0x9c;
            pbVar12 = pbStack_c;
            while( true ) {
              iVar14 = iVar14 + -2;
              pbVar12 = pbVar12 + -0x70;
              iVar25 = iVar24 + -1;
              iVar19 = iVar17;
              if (iVar24 == 0) break;
              *(undefined2 *)((int)_DAT_451b_001a + local_e + iVar14) = 30000;
              iVar24 = iVar25;
              if ((((*(int *)((code *)param_1 + 4) != iVar17) &&
                   (*(int *)((code *)param_1 + 6) != iVar25)) &&
                  ((local_6._2_2_ != (code *)0x0 && ((*pbVar12 & 0x20) == 0)))) ||
                 ((*(byte *)((int)local_10 + iVar25 + iVar11) & 8) == 0)) {
                *(undefined2 *)((int)_DAT_451b_001a + local_e + iVar14) = 0x7531;
              }
            }
          }
          return (code *)0x0;
        }
        pbStack_12 = (byte *)&DAT_451b_0550;
        local_14 = -0x7780;
        puStack_16 = (undefined2 *)&DAT_451b_04e0;
        iVar19 = 0x70;
        while( true ) {
          iVar11 = (int)path_cost_grid;
          pbStack_12 = pbStack_12 + -1;
          local_14 = local_14 + -0x138;
          puStack_16 = puStack_16 + -0x4e;
          iVar17 = iVar19 + -1;
          if (iVar19 == 0) break;
          iVar14 = 0x138;
          iVar24 = 0x9c;
          pbVar12 = pbStack_12;
          while( true ) {
            iVar14 = iVar14 + -2;
            pbVar12 = pbVar12 + -0x70;
            iVar25 = iVar24 + -1;
            iVar19 = iVar17;
            if (iVar24 == 0) break;
            *(undefined2 *)((int)_DAT_451b_001a + local_14 + iVar14) = 30000;
            iVar24 = iVar25;
            if (((((*(int *)((code *)param_1 + 4) != iVar17) &&
                  (*(int *)((code *)param_1 + 6) != iVar25)) && (local_6._2_2_ != (code *)0x0)) &&
                ((*pbVar12 & 0x20) == 0)) ||
               ((pbVar20 = (byte *)((int)puStack_16 + iVar25 + iVar11), (*pbVar20 & 0x80) != 0 &&
                ((*pbVar20 & 7) == 0)))) {
              *(undefined2 *)((int)_DAT_451b_001a + local_14 + iVar14) = 0x7531;
            }
          }
        }
        return (code *)0x0;
      }
      DAT_451b_000e = 10;
      pcVar13 = (code *)(*(int *)((code *)param_1 + 6) * 0xe0 + *(int *)((code *)param_1 + 4) * 2);
      pcVar18 = (code *)0x2e1d;
    }
    if ((*(char *)((byte)*pcVar13 + 0x710) == '\x03') ||
       (*(char *)((byte)*pcVar13 + 0x710) == '\x02')) {
      DAT_451b_000e = 0x14;
    }
    local_6._0_2_ = (code *)&DAT_451b_0550;
    uStack_8 = -0x7780;
    puStack_a = (undefined2 *)&DAT_451b_04e0;
    iVar19 = 0x70;
    while( true ) {
      iVar11 = (int)path_cost_grid;
      local_6._0_2_ = (code *)local_6 + -1;
      uStack_8 = uStack_8 + -0x138;
      puStack_a = puStack_a + -0x4e;
      iVar17 = iVar19 + -1;
      if (iVar19 == 0) break;
      iVar14 = 0x138;
      iVar24 = 0x9c;
      pbVar12 = (byte *)(code *)local_6;
      while( true ) {
        iVar14 = iVar14 + -2;
        pbVar12 = pbVar12 + -0x70;
        iVar25 = iVar24 + -1;
        iVar19 = iVar17;
        if (iVar24 == 0) break;
        *(undefined2 *)((int)_DAT_451b_001a + uStack_8 + iVar14) = 30000;
        iVar24 = iVar25;
        if ((((local_6._2_2_ != (code *)0x0) && ((*pbVar12 & 0x20) == 0)) &&
            ((*(int *)((code *)param_1 + 4) != iVar17 || (*(int *)((code *)param_1 + 6) != iVar25)))
            ) || ((*(byte *)((int)puStack_a + iVar25 + iVar11) & 7) == 0)) {
          *(undefined2 *)((int)_DAT_451b_001a + uStack_8 + iVar14) = 0x7531;
        }
      }
    }
    return (code *)0x0;
  case 6:
    if ((!bVar36 && bVar39 == bVar37) && (pbVar12 != (byte *)0x0)) {
      *(code **)pbVar12 = pcVar13;
      *(code **)(pbVar12 + 2) = pcVar18;
    }
    pcVar18 = pcVar18 + ((code *)0xffef < pcVar13);
    pcVar26 = (code *)FUN_255e_07c1();
    if ((pcVar26 == (code *)0x0) && (pcVar18 == (code *)0x0)) {
      pcVar6 = (code *)swi(0x21);
      (*pcVar6)();
      return (code *)0x0;
    }
    pcVar6 = (code *)swi(0x21);
    (*pcVar6)();
    iVar19 = -0x8000;
    do {
      pcVar6 = (code *)swi(0x21);
      iVar11 = (*pcVar6)();
    } while (iVar11 == iVar19);
    pcVar6 = (code *)swi(0x21);
    (*pcVar6)();
    *param_3 = (int)(pcVar18 + ((uint)(pcVar26 + 0xf) >> 4));
    return pcVar26;
  case 7:
    while( true ) {
      *(uint *)(pcVar22 + -99) = *(uint *)(pcVar22 + -99) >> 1;
      if ((byte)((byte)pcVar13 | 0x8b) == 0) break;
      FUN_1d28_1314();
      uVar21 = 0x20;
      if (DAT_4125_336d < 0x21) {
        uVar21 = DAT_4125_336d;
      }
      uVar15 = uVar21 * 0x10;
      pcVar6 = (code *)swi(0x15);
      DAT_4125_3346 = uVar15;
      DAT_4125_334e = uVar15;
      pcVar26 = pcVar18;
      (*pcVar6)();
      bVar36 = CARRY2(DAT_4125_3350,uVar15);
      DAT_4125_3350 = DAT_4125_3350 + uVar15;
      DAT_4125_3352 = DAT_4125_3352 + bVar36;
      DAT_4125_336d = DAT_4125_336d - uVar21;
      pcVar13 = *(code **)(pcVar22 + 0xe);
      pcVar18 = *(code **)pcVar26;
      *(int *)pcVar26 = -1;
      *(int *)(pcVar22 + 0xc) = *(int *)(pcVar22 + 0xc) + 1;
    }
    return (code *)((uint)pcVar13 | 0x8b);
  case 8:
    pbVar12[5] = pbVar12[5] + (char)((uint)pcVar18 >> 8);
    FUN_8611_006c();
    if (local_6._2_2_ == (code *)0x2) {
      FUN_8c07_08fb();
      FUN_1b62_08b3();
      if ((opt_tutorial != 0) && (iVar19 = game_turn(), iVar19 == 1)) {
        get_file_string(0x19,9);
        FUN_8065_168d();
      }
      local_6._2_2_ = (code *)&DAT_4125_0002;
    }
    else {
      FUN_8065_0174();
      if (opt_tutorial != 0) {
        if ((DAT_3c04_0134 & 0x10) == 0) {
          for (iVar19 = 0; iVar19 < 8; iVar19 = iVar19 + 1) {
            local_6 = (code *)CONCAT22(local_6._2_2_,
                                       (code *)(*DAT_451b_1f02 +
                                               *(int *)((int)(undefined2 *)&DAT_4125_2b4b +
                                                       iVar19 * 2 + 1)));
            uStack_8 = ((int *)DAT_451b_1f02)[1] + ((undefined2 *)&DAT_4125_2b5c)[iVar19];
            puStack_a = (undefined2 *)FUN_828e_044a();
            if ((-1 < (int)puStack_a) &&
               (*(char *)((int)puStack_a * 0x41 + 0x1592) != current_player)) {
              DAT_3c04_0134 = DAT_3c04_0134 | 0x10;
              get_file_string(0x19,4);
              FUN_8065_168d();
              break;
            }
          }
        }
        if (-1 < (char)DAT_3c04_0134) {
          uVar34 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
          if ((*(char *)(*(byte *)(((int *)DAT_451b_1f02)[1] * 0xe0 + *DAT_451b_1f02 * 2) + 0x710)
               == '\v') && ((char)((int *)DAT_451b_1f02)[2] == '\x1c')) {
            DAT_3c04_0134 = DAT_3c04_0134 | 0x80;
            get_file_string(0x19,5);
            FUN_8065_168d();
          }
        }
      }
    }
    return local_6._2_2_;
  case 9:
    FUN_7204_033b();
    pcVar26 = (code *)FUN_17be_0b0a();
    return pcVar26;
  case 10:
    return pcVar13;
  case 0xc:
    *(int *)pcVar26 = *(int *)pcVar26 + 1;
LAB_1000_2176:
    pcStack_2a = (code *)param_2;
    pcStack_2c = pcVar33;
    pcStack_28 = pcVar26;
    __scantol(in_stack_00000002,(code *)param_1);
    if (0 < (int)puStack_a) {
      if (((ulong)local_6 & 0x1000000) != 0) goto LAB_1000_2061;
      local_6 = (code *)CONCAT22(local_6._2_2_,(code *)local_6 + 1);
LAB_1000_21a9:
      uVar40 = __scan_next_arg();
      *(int *)pbVar12 = (int)uVar40;
      if (((ulong)local_6 & 0x4000000) != 0) {
        *(int *)(pbVar12 + 2) = (int)((ulong)uVar40 >> 0x10);
      }
LAB_1000_2061:
      pbVar20 = param_2._2_2_;
LAB_1000_2064:
      pbVar1 = pbVar20;
      pbVar20 = pbVar20 + 1;
      bVar16 = *pbVar1;
      if (bVar16 == 0) goto switchD_1000_2105_caseD_0;
      if (bVar16 != 0x25) goto LAB_1000_2071;
      pbStack_c = (byte *)0xffff;
      local_6 = (code *)CONCAT13(0x20,(undefined3)local_6);
      pbVar30 = pbVar20;
LAB_1000_20ea:
      pbVar8 = pbStack_c;
      pbVar20 = pbVar30 + 1;
      bVar16 = *pbVar30;
      pbVar12 = (byte *)(int)(char)bVar16;
      param_2 = (code *)CONCAT22(pbVar20,(code *)param_2);
      if ((int)pbVar12 < 0) goto switchD_1000_2105_caseD_0;
      piVar32 = (int *)param_3;
      switch(pbVar12[0x3d88]) {
      case 0:
      case 1:
      case 2:
        goto switchD_1000_2105_caseD_0;
      case 3:
        goto switchD_1000_2105_caseD_3;
      case 4:
        local_6 = (code *)((ulong)local_6 | 0x1000000);
        pbVar30 = pbVar20;
        goto LAB_1000_20ea;
      case 5:
        LOCK();
        pbStack_c = pbVar12 + -0x30;
        UNLOCK();
        pbVar30 = pbVar20;
        if (-1 < (int)pbVar8) {
          pbStack_c = pbVar12 + -0x30 + (int)pbVar8 * 10;
        }
        goto LAB_1000_20ea;
      case 6:
        goto switchD_1000_2105_caseD_6;
      case 7:
      case 8:
        pcVar26 = (code *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 6);
        break;
      case 9:
        pcVar26 = (code *)0x0;
        break;
      case 10:
        pcStack_28 = (code *)param_2;
        pcStack_2c = (code *)param_1;
        pcStack_2a = pcVar33;
        __scantod(in_stack_00000002);
        cVar38 = '\0';
        cVar10 = (int)puStack_a < 0;
        if ((int)puStack_a < 1) {
          __scanpop();
          if (cVar38 == cVar10) goto switchD_1000_2105_caseD_0;
          goto switchD_1000_2105_default;
        }
        if (((ulong)local_6 & 0x1000000) == 0) {
          __scan_next_arg();
          local_6 = (code *)CONCAT22(local_6._2_2_,(code *)local_6 + 1);
          __scanrslt();
        }
        else {
          __scanpop();
        }
        goto LAB_1000_2061;
      case 0xb:
        local_6 = (code *)((ulong)local_6 | 0x8000000);
        pbVar30 = pbVar20;
        goto LAB_1000_20ea;
      case 0xc:
        local_6 = (code *)((ulong)local_6 | 0x2000000);
        pbVar30 = pbVar20;
        goto LAB_1000_20ea;
      case 0xd:
        local_6 = (code *)((ulong)local_6 | 0x4000000);
        pbVar30 = pbVar20;
        goto LAB_1000_20ea;
      case 0xe:
        pcVar26 = (code *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 4);
        break;
      case 0xf:
        iVar19 = 0x22a7;
        goto LAB_1000_2464;
      case 0x10:
        goto switchD_1000_2105_caseD_10;
      case 0x11:
        pbVar30 = pbVar20;
        if (((ulong)local_6 & 0x1000000) == 0) goto LAB_1000_21a9;
        goto LAB_1000_20ea;
      case 0x12:
        pcVar26 = (code *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0xc);
        break;
      case 0x13:
        iVar19 = 0x21c3;
        goto LAB_1000_2464;
      case 0x14:
        local_6 = (code *)((ulong)local_6 & 0xdfffffff);
        pbVar30 = pbVar20;
        goto LAB_1000_20ea;
      case 0x15:
        goto switchD_1000_2105_caseD_15;
      default:
        goto switchD_1000_2105_default;
      }
      if ((((uint)pbVar12 & 0x20) == 0) &&
         (pbVar12 != (byte *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x29))) {
        local_6 = (code *)((ulong)local_6 | 0x4000000);
      }
      goto LAB_1000_2176;
    }
    if ((int)puStack_a < 0) {
switchD_1000_2105_default:
      (*(code *)param_1)();
      local_6 = (code *)(ulong)((int)(code *)local_6 - (uint)((code *)local_6 == (code *)0x0));
    }
switchD_1000_2105_caseD_0:
    return (code *)local_6;
  case 0xd:
    while( true ) {
      iVar19 = in_CX * 0x21;
      if (*(code **)(pcVar22 + iVar19) == (code *)param_1) {
        piVar7 = (int *)*(undefined4 *)&DAT_451b_0ce0;
        uVar34 = (undefined2)((ulong)piVar7 >> 0x10);
        local_6 = (code *)CONCAT22(((int *)piVar7)[1],*piVar7 + in_CX * 0x21);
        *(undefined1 *)((int)**(undefined4 **)(undefined4 *)&DAT_451b_0ce0 + in_CX * 0x21 + 4) =
             param_1._2_1_;
        *(undefined1 *)((int)*(undefined4 *)*(undefined4 *)&DAT_451b_0ce0 + in_CX * 0x21 + 5) = 1;
        pcVar26 = (code *)FUN_1a0a_0005();
        return pcVar26;
      }
      in_CX = in_CX + 1;
      if (*(int *)*(undefined4 *)((int)*(undefined4 *)&DAT_451b_0ce0 + 8) <= in_CX) break;
      piVar32 = (int *)((ulong)*(code **)*(undefined4 *)&DAT_451b_0ce0 >> 0x10);
      pcVar22 = (code *)*(code **)*(undefined4 *)&DAT_451b_0ce0;
    }
    return *(code **)(pcVar22 + iVar19);
  case 0xe:
    bVar16 = (byte)((uint)in_CX >> 8);
    *(code **)(pcVar22 + (int)pcVar26) =
         pcVar18 + (uint)CARRY1((byte)pcVar13,bVar16) + *(int *)(pcVar22 + (int)pcVar26);
    local_6 = (code *)(ulong)CONCAT12((byte)pcVar13 + bVar16,(code *)local_6);
    uVar21 = __wherexy();
    local_6 = (code *)CONCAT22(local_6._2_2_,(code *)(uVar21 >> 8));
    while( true ) {
      pcVar6 = param_2;
      pcVar23 = pcVar26 + -1;
      if (pcVar26 == (code *)0x0) {
        __video_int();
        return (code *)(uStack_8 >> 8);
      }
      param_2 = (code *)CONCAT22(param_2._2_2_,(code *)param_2 + 1);
      cVar5 = *pcVar6;
      uStack_8 = CONCAT11(cVar5,(undefined1)uStack_8);
      if ((byte)cVar5 - 7 < 7) break;
      if ((DAT_4125_3e7b == '\0') && (DAT_4125_3e81 != 0)) {
        puStack_a = (undefined2 *)CONCAT11(DAT_4125_3e76,cVar5);
        __screen_offset();
        pcStack_28 = (code *)((char *)s_Can_t_allocate_OS_bitmap_4125_2f1c + 8);
        __vram_copy();
      }
      else {
        __video_int();
        __video_int();
      }
      iVar19 = (int)local_6._2_2_ + 1;
      local_6 = (code *)CONCAT22(iVar19,(code *)local_6);
      if ((int)(uint)DAT_4125_3e74 < iVar19) {
        local_6 = (code *)(ulong)CONCAT12(DAT_4125_3e72,(code *)local_6 + DAT_4125_3e70);
      }
      pcVar26 = pcVar23;
      if ((int)(uint)DAT_4125_3e75 < (int)(code *)local_6) {
        pcStack_28 = (code *)0x1;
        pcStack_2a = (code *)0x2f82;
        __scroll();
        local_6 = (code *)CONCAT22(local_6._2_2_,(code *)local_6 + -1);
      }
    }
    iVar19 = ((byte)cVar5 - 7) * 2;
switchD_1000_2ec3_switchD:
                    /* WARNING: Could not recover jumptable at 0x00012ec3. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    pcVar26 = (code *)(*(code *)*(undefined2 *)((int)(undefined4 *)&DAT_4125_2fa6 + iVar19 + 2))();
    return pcVar26;
  }
  DAT_4125_337e = pcVar9;
  pcVar13 = (code *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 6);
  DAT_4125_338c = 1;
LAB_1ea3_01a7:
  if (6 < DAT_4125_3378) {
    DAT_4125_3378 = 7;
  }
  return pcVar13;
switchD_1000_2105_caseD_15:
  local_6 = (code *)((ulong)local_6 | 0x20000000);
  pbVar30 = pbVar20;
  goto LAB_1000_20ea;
switchD_1000_2105_caseD_10:
  ppcVar29 = &pcStack_2c;
  for (iVar19 = 0x10; iVar19 != 0; iVar19 = iVar19 + -1) {
    ppcVar4 = ppcVar29;
    ppcVar29 = ppcVar29 + 1;
    *ppcVar4 = (code *)0x0;
  }
  pbVar12 = pbVar30 + 2;
  bVar16 = *pbVar20;
  local_6 = (code *)((ulong)local_6 & 0xefffffff);
  if (bVar16 == 0x5e) {
    local_6 = (code *)((ulong)local_6 | 0x10000000);
    pbVar1 = pbVar12;
    pbVar12 = pbVar30 + 3;
    bVar16 = *pbVar1;
  }
  uVar21 = (uint)bVar16;
LAB_1000_236b:
  pbVar20 = pbVar12;
  pbVar30 = (byte *)(uVar21 >> 3);
  bVar16 = '\x01' << ((byte)uVar21 & 7);
  iVar19 = (uint)bVar16 << 8;
  *(byte *)((int)&pcStack_2c + (int)pbVar30) = *(byte *)((int)&pcStack_2c + (int)pbVar30) | bVar16;
  uVar15 = uVar21;
  while( true ) {
    pbVar12 = pbVar20 + 1;
    bVar16 = *pbVar20;
    uVar21 = (uint)bVar16;
    if (bVar16 == 0) goto switchD_1000_2105_caseD_0;
    if (bVar16 == 0x5d) break;
    if (((bVar16 != 0x2d) || (bVar16 = (byte)uVar15, *pbVar12 < bVar16)) || (*pbVar12 == 0x5d))
    goto LAB_1000_236b;
    pbVar20 = pbVar20 + 2;
    cVar10 = *pbVar12 - bVar16;
    if (cVar10 != '\0') {
      uVar15 = (uint)(byte)(bVar16 + cVar10);
      do {
        bVar36 = iVar19 < 0;
        bVar16 = (char)((uint)iVar19 >> 8) << 1 | bVar36;
        iVar19 = (uint)bVar16 << 8;
        pbVar30 = pbVar30 + bVar36;
        *(byte *)((int)&pcStack_2c + (int)pbVar30) =
             *(byte *)((int)&pcStack_2c + (int)pbVar30) | bVar16;
        cVar10 = cVar10 + -1;
      } while (cVar10 != '\0');
    }
  }
  param_2 = (code *)CONCAT22(pbVar12,(code *)param_2);
  pbVar12 = (byte *)((uint)pbStack_c & 0x7fff);
  pbStack_c = pbVar12;
  if (((ulong)local_6 & 0x1000000) == 0) {
    __scan_next_arg();
  }
  while (pbVar20 = pbVar12 + -1, 0 < (int)pbVar12) {
    uStack_8 = uStack_8 + 1;
    uVar21 = (*in_stack_00000002)();
    if ((int)uVar21 < 0) {
      if (((int)pbVar12 < (int)pbStack_c) && (((ulong)local_6 & 0x1000000) == 0)) {
        *pbVar30 = 0;
        local_6 = (code *)CONCAT22(local_6._2_2_,(code *)local_6 + 1);
      }
      goto switchD_1000_2105_default;
    }
    if ((*(byte *)((int)&pcStack_2c + (uVar21 >> 3)) & '\x01' << ((byte)uVar21 & 7)) == 0) {
      if (((ulong)local_6 & 0x10000000) == 0) goto LAB_1000_2406;
    }
    else if (((ulong)local_6 & 0x10000000) != 0) {
LAB_1000_2406:
      (*(code *)param_1)();
      uStack_8 = uStack_8 - 1;
      if ((int)pbVar12 < (int)pbStack_c) break;
      goto LAB_1000_2427;
    }
    pbVar12 = pbVar20;
    if (((ulong)local_6 & 0x1000000) == 0) {
      pbVar1 = pbVar30;
      pbVar30 = pbVar30 + 1;
      *pbVar1 = (byte)uVar21;
    }
  }
  if (((ulong)local_6 & 0x1000000) == 0) {
    local_6 = (code *)CONCAT22(local_6._2_2_,(code *)local_6 + 1);
LAB_1000_2427:
    if (((ulong)local_6 & 0x1000000) == 0) {
      *pbVar30 = 0;
    }
  }
  goto LAB_1000_2061;
LAB_1000_2464:
  uStack_8 = uStack_8 + 1;
  iVar11 = (*in_stack_00000002)();
  if (iVar11 < 1) {
    if (iVar11 != 0) goto switchD_1000_2105_default;
LAB_1000_2483:
                    /* WARNING: Could not recover jumptable at 0x00012487. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    pcVar26 = (code *)(*(code *)(iVar19 + 3))();
    return pcVar26;
  }
  if (((char)iVar11 < '\0') || (*(char *)(iVar11 + 0x3d88) != '\x01')) goto LAB_1000_2483;
  goto LAB_1000_2464;
switchD_1000_2105_caseD_6:
  if (((ulong)local_6 & 0x1000000) == 0) {
    __scan_next_arg();
  }
  pbVar20 = pbStack_c;
  if ((int)pbStack_c < 0) {
    pbVar20 = (byte *)0x1;
  }
  if (pbStack_c != (byte *)0x0) goto LAB_1000_231d;
  goto LAB_1000_233b;
  while( true ) {
    if (((ulong)local_6 & 0x1000000) == 0) {
      pbVar1 = pbVar12;
      pbVar12 = pbVar12 + 1;
      *pbVar1 = (byte)iVar19;
    }
    pbVar30 = pbVar20 + -1;
    bVar36 = (int)pbVar20 < 1;
    pbVar20 = pbVar30;
    if (pbVar30 == (byte *)0x0 || bVar36) break;
LAB_1000_231d:
    uStack_8 = uStack_8 + 1;
    iVar19 = (*in_stack_00000002)();
    if (iVar19 < 0) goto switchD_1000_2105_default;
  }
LAB_1000_233b:
  if (((ulong)local_6 & 0x1000000) == 0) {
    local_6 = (code *)CONCAT22(local_6._2_2_,(code *)local_6 + 1);
  }
  goto LAB_1000_2061;
switchD_1000_2105_caseD_3:
LAB_1000_2071:
  iVar19 = (int)(char)bVar16;
  uStack_8 = uStack_8 + 1;
  iVar11 = (*in_stack_00000002)();
  if (iVar11 < 0) goto switchD_1000_2105_default;
  iVar11 = (int)(char)iVar11;
  if ((iVar19 < 0) || (*(char *)(iVar19 + 0x3d88) != '\x01')) {
    if (iVar11 != iVar19) {
      (*(code *)param_1)();
      goto switchD_1000_2105_caseD_0;
    }
  }
  else {
    while ((-1 < (char)iVar11 && (*(char *)(iVar11 + 0x3d88) == '\x01'))) {
      uStack_8 = uStack_8 + 1;
      iVar11 = (*in_stack_00000002)();
      if (iVar11 < 1) goto switchD_1000_2105_default;
    }
    (*(code *)param_1)();
    uStack_8 = uStack_8 - 1;
  }
  goto LAB_1000_2064;
}


