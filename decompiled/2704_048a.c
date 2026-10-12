
void FUN_2704_048a(uint *param_1,uint *param_2,uint *param_3)

{
  uint uVar1;
  ulong uVar2;
  long lVar3;
  long lVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  byte bVar15;
  uint uVar14;
  uint uVar16;
  uint uVar17;
  uint uVar18;
  uint uVar19;
  uint uVar20;
  uint uVar21;
  uint uVar22;
  uint uVar23;
  uint uVar24;
  uint uVar25;
  int iVar26;
  uint uVar27;
  uint uVar28;
  undefined2 unaff_ES;
  bool bVar29;
  bool bVar30;
  uint local_e;
  uint local_c;
  byte local_a;
  uint local_8;
  
  uVar5 = param_3[5];
  uVar6 = param_2[5];
  uVar1 = param_2[4];
  local_8 = param_3[4];
  bVar29 = uVar1 < 0x4001;
  if (0x4000 < (int)uVar1) {
    FUN_2704_0012();
    if (bVar29) {
      return;
    }
    if (0x4000 < (int)param_3[4]) {
      FUN_2704_0e53();
      return;
    }
    goto LAB_2704_0463;
  }
  bVar29 = local_8 < 0x4001;
  if (0x4000 < (int)local_8) {
    FUN_2704_0012();
    if (bVar29) {
      return;
    }
    goto LAB_2704_0463;
  }
  if ((int)uVar1 < -0x3ffe) {
LAB_2704_045d:
    FUN_2704_2714();
    goto LAB_2704_0463;
  }
  if ((int)local_8 < -0x3ffe) goto LAB_2704_0463;
  local_8 = local_8 - uVar1;
  if (0x4000 < (int)local_8) goto LAB_2704_045d;
  if ((int)local_8 < -0x3ffe) {
    FUN_2704_2714();
    goto LAB_2704_0463;
  }
  uVar1 = param_2[3];
  uVar7 = param_2[2];
  uVar14 = param_2[1];
  uVar13 = *param_2;
  local_a = 0;
  uVar25 = param_3[3];
  uVar20 = param_3[2];
  if ((uVar7 == 0) && (uVar14 == 0 && uVar13 == 0)) {
    local_a = uVar1 <= uVar25;
    if ((bool)local_a) {
      uVar25 = uVar25 - uVar1;
    }
    local_c = (uint)(CONCAT22(uVar25,uVar20) / (ulong)uVar1);
    uVar2 = CONCAT22(uVar25,uVar20) % (ulong)uVar1 << 0x10 | (ulong)param_3[1];
    local_e = (uint)(uVar2 / uVar1);
    uVar2 = uVar2 % (ulong)uVar1 << 0x10 | (ulong)*param_3;
    uVar13 = (uint)(uVar2 / uVar1);
    uVar2 = uVar2 % (ulong)uVar1 << 0x10;
    uVar7 = (uint)(uVar2 / uVar1);
    bVar15 = (byte)(uVar2 % (ulong)uVar1 >> 8);
  }
  else {
    uVar8 = param_3[1];
    uVar16 = *param_3;
    if (uVar25 < uVar1) {
LAB_2704_056a:
      uVar2 = CONCAT22(uVar25,uVar20) / (ulong)uVar1;
      local_c = (uint)uVar2;
      lVar3 = (uVar2 & 0xffff) * (ulong)uVar1;
      uVar22 = (uint)lVar3;
      uVar28 = uVar20 - uVar22;
      lVar4 = (ulong)uVar7 * (uVar2 & 0xffff);
      uVar18 = (uint)((ulong)lVar4 >> 0x10);
      uVar9 = (uint)lVar4;
      uVar10 = uVar8 - uVar9;
      uVar11 = (uint)(uVar8 < uVar9);
      uVar21 = uVar28 - uVar18;
      uVar23 = uVar21 - uVar11;
      lVar4 = (ulong)uVar14 * (uVar2 & 0xffff);
      uVar9 = (uint)((ulong)lVar4 >> 0x10);
      uVar8 = (uint)lVar4;
      uVar27 = uVar16 - uVar8;
      uVar8 = (uint)(uVar16 < uVar8);
      uVar16 = uVar10 - uVar9;
      uVar17 = uVar16 - uVar8;
      uVar12 = (uint)(uVar10 < uVar9 || uVar16 < uVar8);
      uVar24 = uVar23 - uVar12;
      lVar4 = (ulong)uVar13 * (uVar2 & 0xffff);
      uVar19 = (uint)((ulong)lVar4 >> 0x10);
      iVar26 = (int)lVar4;
      uVar8 = -iVar26;
      uVar16 = (uint)(iVar26 != 0);
      uVar10 = uVar27 - uVar19;
      uVar9 = uVar10 - uVar16;
      uVar10 = (uint)(uVar27 < uVar19 || uVar10 < uVar16);
      uVar16 = uVar17 - uVar10;
      uVar17 = (uint)(uVar17 < uVar10);
      uVar10 = uVar24 - uVar17;
      if ((int)(uint)(uVar24 < uVar17) <=
          (int)((((uVar25 - (int)((ulong)lVar3 >> 0x10)) - (uint)(uVar20 < uVar22)) -
                (uint)(uVar28 < uVar18 || uVar21 < uVar11)) - (uint)(uVar23 < uVar12)))
      goto LAB_2704_0636;
      if (uVar1 <= -uVar10) {
        bVar29 = local_c == 0;
        local_c = local_c - 1;
        local_a = local_a - bVar29;
        bVar29 = CARRY2(uVar8,uVar13);
        uVar8 = uVar8 + uVar13;
        bVar30 = CARRY2(uVar9,uVar14);
        uVar25 = uVar9 + uVar14;
        uVar9 = uVar25 + bVar29;
        uVar25 = (uint)(bVar30 || CARRY2(uVar25,(uint)bVar29));
        bVar29 = CARRY2(uVar16,uVar7);
        uVar20 = uVar16 + uVar7;
        uVar16 = uVar20 + uVar25;
        uVar25 = (uint)(bVar29 || CARRY2(uVar20,uVar25));
        bVar29 = SCARRY2(uVar10,uVar1);
        iVar26 = uVar10 + uVar1;
        uVar10 = iVar26 + uVar25;
        if ((bVar29 != SCARRY2(iVar26,uVar25)) == (int)uVar10 < 0) goto LAB_2704_0636;
      }
LAB_2704_05cd:
      uVar2 = CONCAT22(-(uint)(uVar16 != 0) - uVar10,-uVar16) / (ulong)uVar1;
      lVar4 = (uVar2 & 0xffff) * (ulong)uVar1 + CONCAT22(uVar10,uVar16);
      uVar10 = (uint)lVar4;
      lVar3 = (ulong)uVar7 * (uVar2 & 0xffff);
      uVar11 = (uint)((ulong)lVar3 >> 0x10);
      uVar25 = (uint)lVar3;
      uVar17 = uVar9 + uVar25;
      uVar25 = (uint)CARRY2(uVar9,uVar25);
      uVar16 = uVar10 + uVar11;
      uVar9 = uVar16 + uVar25;
      lVar3 = (ulong)uVar14 * (uVar2 & 0xffff);
      uVar12 = (uint)((ulong)lVar3 >> 0x10);
      uVar20 = (uint)lVar3;
      uVar22 = uVar8 + uVar20;
      uVar20 = (uint)CARRY2(uVar8,uVar20);
      uVar8 = uVar17 + uVar12;
      uVar21 = uVar8 + uVar20;
      uVar20 = (uint)(CARRY2(uVar17,uVar12) || CARRY2(uVar8,uVar20));
      uVar8 = uVar9 + uVar20;
      iVar26 = (int)((ulong)lVar4 >> 0x10) + (uint)(CARRY2(uVar10,uVar11) || CARRY2(uVar16,uVar25))
               + (uint)CARRY2(uVar9,uVar20);
      uVar25 = (uint)((ulong)uVar13 * (uVar2 & 0xffff) >> 0x10);
      local_e = -(int)uVar2;
      uVar13 = (uint)((int)uVar2 != 0);
      bVar29 = local_c < uVar13;
      local_c = local_c - uVar13;
      local_a = local_a - bVar29;
      uVar28 = uVar22 + uVar25;
      uVar13 = (uint)CARRY2(uVar22,uVar25);
      uVar18 = uVar21 + uVar13;
      uVar13 = (uint)CARRY2(uVar21,uVar13);
      uVar21 = uVar8 + uVar13;
      uVar13 = (uint)CARRY2(uVar8,uVar13);
      if (SCARRY2(iVar26,uVar13) != (int)(iVar26 + uVar13) < 0) goto LAB_2704_0694;
      if (uVar1 <= uVar21) {
        bVar30 = 0xfffe < local_e;
        local_e = local_e + 1;
        bVar29 = CARRY2(local_c,(uint)bVar30);
        local_c = local_c + bVar30;
        local_a = local_a + bVar29;
        bVar29 = uVar28 < uVar14;
        uVar28 = uVar28 - uVar14;
        bVar30 = uVar18 < uVar7;
        uVar13 = uVar18 - uVar7;
        uVar18 = uVar13 - bVar29;
        uVar13 = (uint)(bVar30 || uVar13 < bVar29);
        bVar29 = SBORROW2(uVar21,uVar1);
        iVar26 = uVar21 - uVar1;
        uVar21 = iVar26 - uVar13;
        if ((bVar29 != SBORROW2(iVar26,uVar13)) != (int)uVar21 < 0) goto LAB_2704_0694;
      }
LAB_2704_06e9:
      uVar2 = CONCAT22(uVar21,uVar18) / (ulong)uVar1;
      uVar13 = (uint)uVar2;
      lVar3 = (uVar2 & 0xffff) * (ulong)uVar1;
      uVar9 = (uint)lVar3;
      uVar11 = uVar18 - uVar9;
      lVar4 = (ulong)uVar7 * (uVar2 & 0xffff);
      uVar10 = (uint)((ulong)lVar4 >> 0x10);
      uVar25 = (uint)lVar4;
      uVar17 = uVar28 - uVar25;
      uVar8 = (uint)(uVar28 < uVar25);
      uVar16 = uVar11 - uVar10;
      uVar12 = uVar16 - uVar8;
      uVar14 = (uint)((ulong)uVar14 * (uVar2 & 0xffff) >> 0x10);
      uVar20 = uVar17 - uVar14;
      uVar14 = (uint)(uVar17 < uVar14);
      uVar25 = uVar12 - uVar14;
      if ((int)(((uVar21 - (int)((ulong)lVar3 >> 0x10)) - (uint)(uVar18 < uVar9)) -
               (uint)(uVar11 < uVar10 || uVar16 < uVar8)) < (int)(uint)(uVar12 < uVar14)) {
        if (uVar1 <= -uVar25) {
          bVar30 = uVar13 == 0;
          uVar13 = uVar13 - 1;
          bVar29 = local_e < bVar30;
          local_e = local_e - bVar30;
          uVar14 = (uint)bVar29;
          bVar29 = local_c < uVar14;
          local_c = local_c - uVar14;
          local_a = local_a - bVar29;
          bVar29 = CARRY2(uVar20,uVar7);
          uVar20 = uVar20 + uVar7;
          bVar30 = SCARRY2(uVar25,uVar1);
          iVar26 = uVar25 + uVar1;
          uVar25 = iVar26 + (uint)bVar29;
          if ((bVar30 != SCARRY2(iVar26,(uint)bVar29)) == (int)uVar25 < 0) goto LAB_2704_0773;
        }
LAB_2704_0731:
        uVar7 = uVar1 - (uVar7 >> 1);
        uVar25 = -(uint)(uVar7 < uVar20) - uVar25;
        if (uVar25 < uVar1) {
          uVar2 = CONCAT22(uVar25,uVar7 - uVar20);
          iVar26 = (int)(uVar2 / uVar1);
          bVar15 = (byte)(-((int)(uVar2 % (ulong)uVar1) - uVar1) >> 8);
          bVar29 = iVar26 != 0;
          uVar7 = -iVar26;
        }
        else {
          uVar7 = 0;
          bVar15 = 0;
          bVar29 = true;
        }
        bVar30 = uVar13 < bVar29;
        uVar13 = uVar13 - bVar29;
        if (bVar30) {
          bVar29 = local_e == 0;
          local_e = local_e - 1;
          bVar30 = local_c < bVar29;
          local_c = local_c - bVar29;
          local_a = local_a - bVar30;
        }
        goto LAB_2704_0791;
      }
    }
    else {
      local_a = 1;
      bVar29 = uVar16 < uVar13;
      uVar16 = uVar16 - uVar13;
      bVar30 = uVar8 < uVar14;
      uVar9 = uVar8 - uVar14;
      uVar8 = uVar9 - bVar29;
      uVar9 = (uint)(bVar30 || uVar9 < bVar29);
      bVar29 = uVar20 < uVar7;
      uVar10 = uVar20 - uVar7;
      uVar20 = uVar10 - uVar9;
      uVar9 = (uint)(bVar29 || uVar10 < uVar9);
      bVar29 = uVar1 <= uVar25;
      uVar10 = uVar25 - uVar1;
      uVar25 = uVar10 - uVar9;
      if (bVar29 && uVar9 <= uVar10) goto LAB_2704_056a;
      uVar2 = CONCAT22(-(uint)(uVar20 != 0) - uVar25,-uVar20) / (ulong)uVar1;
      lVar4 = (uVar2 & 0xffff) * (ulong)uVar1 + CONCAT22(uVar25,uVar20);
      uVar21 = (uint)lVar4;
      lVar3 = (ulong)uVar7 * (uVar2 & 0xffff);
      uVar11 = (uint)((ulong)lVar3 >> 0x10);
      uVar25 = (uint)lVar3;
      uVar9 = uVar8 + uVar25;
      uVar25 = (uint)CARRY2(uVar8,uVar25);
      uVar8 = uVar21 + uVar11;
      uVar22 = uVar8 + uVar25;
      lVar3 = (ulong)uVar14 * (uVar2 & 0xffff);
      uVar12 = (uint)((ulong)lVar3 >> 0x10);
      uVar20 = (uint)lVar3;
      uVar17 = uVar16 + uVar20;
      uVar20 = (uint)CARRY2(uVar16,uVar20);
      uVar16 = uVar9 + uVar12;
      uVar10 = uVar16 + uVar20;
      uVar20 = (uint)(CARRY2(uVar9,uVar12) || CARRY2(uVar16,uVar20));
      uVar12 = uVar22 + uVar20;
      iVar26 = (int)((ulong)lVar4 >> 0x10) + (uint)(CARRY2(uVar21,uVar11) || CARRY2(uVar8,uVar25)) +
               (uint)CARRY2(uVar22,uVar20);
      lVar3 = (ulong)uVar13 * (uVar2 & 0xffff);
      uVar25 = (uint)((ulong)lVar3 >> 0x10);
      uVar8 = (uint)lVar3;
      local_c = -(int)uVar2;
      local_a = 1 - ((int)uVar2 != 0);
      uVar9 = uVar17 + uVar25;
      uVar25 = (uint)CARRY2(uVar17,uVar25);
      uVar16 = uVar10 + uVar25;
      uVar25 = (uint)CARRY2(uVar10,uVar25);
      uVar10 = uVar12 + uVar25;
      uVar25 = (uint)CARRY2(uVar12,uVar25);
      bVar29 = SCARRY2(iVar26,uVar25);
      bVar30 = (int)(iVar26 + uVar25) < 0;
      if (bVar29 != bVar30) {
LAB_2704_0565:
        if (bVar29 == bVar30) goto LAB_2704_0636;
        goto LAB_2704_05cd;
      }
      if (uVar1 <= uVar10) {
        bVar29 = 0xfffe < local_c;
        local_c = local_c + 1;
        local_a = local_a + bVar29;
        bVar29 = uVar8 < uVar13;
        uVar8 = uVar8 - uVar13;
        bVar30 = uVar9 < uVar14;
        uVar25 = uVar9 - uVar14;
        uVar9 = uVar25 - bVar29;
        uVar25 = (uint)(bVar30 || uVar25 < bVar29);
        bVar29 = uVar16 < uVar7;
        uVar20 = uVar16 - uVar7;
        uVar16 = uVar20 - uVar25;
        uVar25 = (uint)(bVar29 || uVar20 < uVar25);
        bVar29 = SBORROW2(uVar10,uVar1) != SBORROW2(uVar10 - uVar1,uVar25);
        uVar10 = (uVar10 - uVar1) - uVar25;
        bVar30 = (int)uVar10 < 0;
        goto LAB_2704_0565;
      }
LAB_2704_0636:
      uVar2 = CONCAT22(uVar10,uVar16) / (ulong)uVar1;
      local_e = (uint)uVar2;
      lVar3 = (uVar2 & 0xffff) * (ulong)uVar1;
      uVar11 = (uint)lVar3;
      uVar12 = uVar16 - uVar11;
      lVar4 = (ulong)uVar7 * (uVar2 & 0xffff);
      uVar22 = (uint)((ulong)lVar4 >> 0x10);
      uVar25 = (uint)lVar4;
      uVar18 = uVar9 - uVar25;
      uVar25 = (uint)(uVar9 < uVar25);
      uVar9 = uVar12 - uVar22;
      uVar17 = uVar9 - uVar25;
      lVar4 = (ulong)uVar14 * (uVar2 & 0xffff);
      uVar21 = (uint)((ulong)lVar4 >> 0x10);
      uVar20 = (uint)lVar4;
      uVar27 = uVar8 - uVar20;
      uVar20 = (uint)(uVar8 < uVar20);
      uVar8 = uVar18 - uVar21;
      uVar19 = uVar8 - uVar20;
      uVar20 = (uint)(uVar18 < uVar21 || uVar8 < uVar20);
      uVar8 = uVar17 - uVar20;
      uVar13 = (uint)((ulong)uVar13 * (uVar2 & 0xffff) >> 0x10);
      uVar28 = uVar27 - uVar13;
      uVar13 = (uint)(uVar27 < uVar13);
      uVar18 = uVar19 - uVar13;
      uVar13 = (uint)(uVar19 < uVar13);
      uVar21 = uVar8 - uVar13;
      if ((int)(uint)(uVar8 < uVar13) <=
          (int)((((uVar10 - (int)((ulong)lVar3 >> 0x10)) - (uint)(uVar16 < uVar11)) -
                (uint)(uVar12 < uVar22 || uVar9 < uVar25)) - (uint)(uVar17 < uVar20)))
      goto LAB_2704_06e9;
      if (uVar1 <= -uVar21) {
        bVar30 = local_e == 0;
        local_e = local_e - 1;
        bVar29 = local_c < bVar30;
        local_c = local_c - bVar30;
        local_a = local_a - bVar29;
        bVar29 = CARRY2(uVar28,uVar14);
        uVar28 = uVar28 + uVar14;
        bVar30 = CARRY2(uVar18,uVar7);
        uVar13 = uVar18 + uVar7;
        uVar18 = uVar13 + bVar29;
        uVar13 = (uint)(bVar30 || CARRY2(uVar13,(uint)bVar29));
        bVar29 = SCARRY2(uVar21,uVar1);
        iVar26 = uVar21 + uVar1;
        uVar21 = iVar26 + uVar13;
        if ((bVar29 != SCARRY2(iVar26,uVar13)) == (int)uVar21 < 0) goto LAB_2704_06e9;
      }
LAB_2704_0694:
      uVar2 = CONCAT22(-(uint)(uVar18 != 0) - uVar21,-uVar18) / (ulong)uVar1;
      lVar4 = (uVar2 & 0xffff) * (ulong)uVar1 + CONCAT22(uVar21,uVar18);
      uVar8 = (uint)lVar4;
      lVar3 = (ulong)uVar7 * (uVar2 & 0xffff);
      uVar20 = (uint)((ulong)lVar3 >> 0x10);
      uVar13 = (uint)lVar3;
      uVar9 = uVar28 + uVar13;
      uVar13 = (uint)CARRY2(uVar28,uVar13);
      uVar25 = uVar8 + uVar20;
      uVar16 = uVar25 + uVar13;
      iVar26 = (int)((ulong)lVar4 >> 0x10) + (uint)(CARRY2(uVar8,uVar20) || CARRY2(uVar25,uVar13));
      uVar25 = (uint)((ulong)uVar14 * (uVar2 & 0xffff) >> 0x10);
      uVar13 = -(int)uVar2;
      uVar14 = (uint)((int)uVar2 != 0);
      bVar29 = local_e < uVar14;
      local_e = local_e - uVar14;
      uVar14 = (uint)bVar29;
      bVar29 = local_c < uVar14;
      local_c = local_c - uVar14;
      local_a = local_a - bVar29;
      uVar20 = uVar9 + uVar25;
      uVar14 = (uint)CARRY2(uVar9,uVar25);
      uVar25 = uVar16 + uVar14;
      uVar14 = (uint)CARRY2(uVar16,uVar14);
      if (SCARRY2(iVar26,uVar14) != (int)(iVar26 + uVar14) < 0) goto LAB_2704_0731;
      if (uVar1 <= uVar25) {
        bVar30 = 0xfffe < uVar13;
        uVar13 = uVar13 + 1;
        bVar29 = CARRY2(local_e,(uint)bVar30);
        local_e = local_e + bVar30;
        uVar14 = (uint)bVar29;
        bVar29 = CARRY2(local_c,uVar14);
        local_c = local_c + uVar14;
        local_a = local_a + bVar29;
        bVar29 = uVar20 < uVar7;
        uVar20 = uVar20 - uVar7;
        bVar30 = SBORROW2(uVar25,uVar1);
        iVar26 = uVar25 - uVar1;
        uVar25 = iVar26 - (uint)bVar29;
        if ((bVar30 != SBORROW2(iVar26,(uint)bVar29)) != (int)uVar25 < 0) goto LAB_2704_0731;
      }
    }
LAB_2704_0773:
    uVar7 = -(uVar7 >> 1);
    uVar14 = ((int)uVar7 >> 0xf) + uVar25 + (uint)CARRY2(uVar7,uVar20);
    if (uVar14 < uVar1) {
      uVar2 = CONCAT22(uVar14,uVar7 + uVar20);
      uVar7 = (uint)(uVar2 / uVar1);
      bVar15 = (byte)(uVar2 % (ulong)uVar1 >> 8);
    }
    else {
      uVar7 = 0;
      bVar15 = 0;
    }
  }
LAB_2704_0791:
  uVar8 = uVar7;
  uVar25 = local_e;
  uVar20 = uVar13;
  uVar14 = local_c;
  if ((bool)(local_a & 1)) {
    uVar14 = (uint)((CONCAT12(local_a,local_c) & 0x1ffff) >> 1);
    uVar25 = (uint)(CONCAT12((local_c & 1) != 0,local_e) >> 1);
    uVar20 = (uint)(CONCAT12((local_e & 1) != 0,uVar13) >> 1);
    uVar8 = (uint)(CONCAT12((uVar13 & 1) != 0,uVar7) >> 1);
    bVar15 = (byte)(CONCAT11((uVar7 & 1) != 0,bVar15) >> 1);
    local_8 = local_8 + 1;
  }
  uVar1 = (uint)((byte)(uVar1 >> 9) < bVar15);
  uVar7 = uVar8 + uVar1;
  if (CARRY2(uVar8,uVar1)) {
    bVar29 = CARRY2(uVar20,uVar7);
    uVar1 = uVar20 + uVar7;
    uVar20 = uVar1 + 1;
    uVar1 = (uint)(bVar29 || 0xfffe < uVar1);
    bVar29 = CARRY2(uVar25,uVar7);
    uVar13 = uVar25 + uVar7;
    uVar25 = uVar13 + uVar1;
    uVar1 = (uint)(bVar29 || CARRY2(uVar13,uVar1));
    bVar29 = CARRY2(uVar14,uVar7);
    uVar13 = uVar14 + uVar7;
    uVar14 = uVar13 + uVar1;
    if (bVar29 || CARRY2(uVar13,uVar1)) {
      uVar14 = (uint)(CONCAT12(bVar29 || CARRY2(uVar13,uVar1),uVar14) >> 1);
      local_8 = local_8 + 1;
    }
  }
  if ((int)local_8 < 0x4001) {
    *param_1 = uVar7;
    param_1[1] = uVar20;
    param_1[2] = uVar25;
    param_1[3] = uVar14;
    param_1[4] = local_8;
    *(byte *)(param_1 + 5) = (byte)uVar5 ^ (byte)uVar6;
    return;
  }
LAB_2704_0463:
  FUN_2704_0e3d();
  return;
}


