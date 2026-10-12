
void FUN_2704_00f3(uint *param_1,uint *param_2,uint *param_3)

{
  ulong uVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  byte bVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint *puVar14;
  int iVar15;
  undefined2 unaff_ES;
  bool bVar16;
  byte local_7;
  uint local_6;
  char local_4;
  
  bVar2 = (byte)param_2[5] ^ (byte)param_3[5];
  uVar7 = param_3[4];
  uVar11 = param_2[4];
  bVar16 = uVar7 < 0x4001;
  puVar14 = param_3;
  if ((int)uVar7 < 0x4001) {
    if (0x4000 < (int)uVar11) {
      bVar16 = false;
      local_7 = FUN_2704_0012();
      puVar14 = param_2;
      if (bVar16) {
        return;
      }
      goto LAB_2704_0099;
    }
    if ((int)uVar7 < (int)uVar11) {
      bVar5 = (byte)param_2[5];
      uVar3 = uVar11;
      uVar11 = uVar7;
      puVar14 = param_2;
    }
    else {
      bVar5 = (byte)param_3[5];
      uVar3 = uVar7;
      param_3 = param_2;
    }
    local_6 = puVar14[4];
    local_7 = bVar5;
    if ((-0x3fff < (int)uVar11) && (uVar3 = uVar3 - uVar11, (int)uVar3 < 0x42)) {
      uVar7 = param_3[1];
      uVar11 = param_3[2];
      uVar9 = param_3[3];
      uVar13 = *param_3;
      local_4 = '\0';
      while (bVar16 = 7 < (int)uVar3, uVar3 = uVar3 - 8, bVar16) {
        local_4 = (char)uVar13;
        uVar13 = CONCAT11((char)uVar7,(char)(uVar13 >> 8));
        uVar7 = CONCAT11((char)uVar11,(char)(uVar7 >> 8));
        uVar11 = CONCAT11((char)uVar9,(char)(uVar11 >> 8));
        uVar9 = uVar9 >> 8;
      }
      for (uVar3 = uVar3 & 7; uVar3 != 0; uVar3 = uVar3 - 1) {
        uVar8 = uVar9 & 1;
        uVar9 = uVar9 >> 1;
        uVar4 = uVar11 & 1;
        local_4 = (char)(CONCAT11((uVar13 & 1) != 0,local_4) >> 1);
        uVar13 = (uint)(CONCAT12((uVar7 & 1) != 0,uVar13) >> 1);
        uVar11 = (uint)(CONCAT12(uVar8 != 0,uVar11) >> 1);
        uVar7 = (uint)(CONCAT12(uVar4 != 0,uVar7) >> 1);
      }
      if (bVar2 != 0) {
        uVar3 = uVar13 - *puVar14;
        uVar8 = (uint)(uVar13 < *puVar14);
        uVar4 = uVar7 - puVar14[1];
        uVar13 = uVar4 - uVar8;
        uVar8 = (uint)(uVar7 < puVar14[1] || uVar4 < uVar8);
        uVar4 = uVar11 - puVar14[2];
        uVar7 = uVar4 - uVar8;
        uVar8 = (uint)(uVar11 < puVar14[2] || uVar4 < uVar8);
        uVar4 = uVar9 - puVar14[3];
        uVar11 = uVar4 - uVar8;
        local_7 = bVar5 ^ 1;
        if (uVar9 < puVar14[3] || uVar4 < uVar8) {
          uVar8 = ~uVar7;
          uVar9 = ~uVar13;
          uVar7 = ~uVar3;
          bVar16 = local_4 == '\0';
          local_4 = -local_4;
          uVar3 = uVar7 + bVar16;
          uVar7 = (uint)CARRY2(uVar7,(uint)bVar16);
          uVar13 = uVar9 + uVar7;
          uVar9 = (uint)CARRY2(uVar9,uVar7);
          uVar7 = uVar8 + uVar9;
          uVar11 = ~uVar11 + (uint)CARRY2(uVar8,uVar9);
          local_7 = bVar5;
        }
        iVar15 = 8;
        do {
          if ((char)(uVar11 >> 8) != '\0') goto LAB_2704_020c;
          uVar11 = CONCAT11((char)uVar11,(char)(uVar7 >> 8));
          uVar7 = CONCAT11((char)uVar7,(char)(uVar13 >> 8));
          uVar13 = CONCAT11((char)uVar13,(char)(uVar3 >> 8));
          uVar3 = CONCAT11((char)uVar3,local_4);
          local_4 = '\0';
          local_6 = local_6 - 8;
          bVar16 = 0 < iVar15;
          iVar15 = iVar15 + -1;
        } while (bVar16);
        local_6 = 0xc001;
        local_7 = 0;
        goto LAB_2704_022e;
      }
      uVar4 = uVar13 + *puVar14;
      uVar3 = (uint)CARRY2(uVar13,*puVar14);
      uVar13 = uVar7 + puVar14[1];
      uVar12 = uVar13 + uVar3;
      uVar7 = (uint)(CARRY2(uVar7,puVar14[1]) || CARRY2(uVar13,uVar3));
      uVar3 = uVar11 + puVar14[2];
      uVar6 = uVar3 + uVar7;
      uVar8 = (uint)(CARRY2(uVar11,puVar14[2]) || CARRY2(uVar3,uVar7));
      bVar16 = CARRY2(uVar9,puVar14[3]);
      uVar9 = uVar9 + puVar14[3];
      uVar10 = uVar9 + uVar8;
      uVar3 = uVar4;
      uVar7 = uVar6;
      uVar11 = uVar10;
      uVar13 = uVar12;
      if (bVar16 || CARRY2(uVar9,uVar8)) {
        uVar11 = (uint)(CONCAT12(bVar16 || CARRY2(uVar9,uVar8),uVar10) >> 1);
        uVar7 = (uint)(CONCAT12((uVar10 & 1) != 0,uVar6) >> 1);
        uVar13 = (uint)(CONCAT12((uVar6 & 1) != 0,uVar12) >> 1);
        uVar3 = (uint)(CONCAT12((uVar12 & 1) != 0,uVar4) >> 1);
        local_4 = (char)(CONCAT11((uVar4 & 1) != 0,local_4) >> 1);
        local_6 = local_6 + 1;
      }
      goto LAB_2704_020e;
    }
  }
  else {
    FUN_2704_0012(param_2);
    if (bVar16) {
      return;
    }
    if ((0x4000 < (int)uVar11) && (bVar2 != 0)) {
      FUN_2704_0e53();
      return;
    }
    local_7 = (byte)param_3[5];
LAB_2704_0099:
    local_6 = 0x4001;
  }
  uVar3 = *puVar14;
  uVar13 = puVar14[1];
  uVar7 = puVar14[2];
  uVar11 = puVar14[3];
LAB_2704_022e:
  *param_1 = uVar3;
  param_1[1] = uVar13;
  param_1[2] = uVar7;
  param_1[3] = uVar11;
  param_1[4] = local_6;
  *(byte *)(param_1 + 5) = local_7;
  return;
LAB_2704_020c:
  for (; -1 < (int)uVar11; uVar11 = uVar11 * 2 + (uint)((uVar1 & 0x10000) != 0)) {
    local_6 = local_6 - 1;
    bVar16 = local_4 < '\0';
    local_4 = local_4 << 1;
    uVar1 = (ulong)CONCAT12(bVar16,uVar3) << 1;
    uVar3 = (uint)uVar1 | (uint)bVar16;
    bVar16 = (uVar1 & 0x10000) != 0;
    uVar1 = (ulong)CONCAT12(bVar16,uVar13) << 1;
    uVar13 = (uint)uVar1 | (uint)bVar16;
    bVar16 = (uVar1 & 0x10000) != 0;
    uVar1 = (ulong)CONCAT12(bVar16,uVar7) << 1;
    uVar7 = (uint)uVar1 | (uint)bVar16;
  }
LAB_2704_020e:
  bVar16 = CARRY2(uVar3,(uint)(local_4 < '\0'));
  uVar3 = uVar3 + (local_4 < '\0');
  uVar9 = (uint)bVar16;
  uVar8 = (uint)CARRY2(uVar13,uVar9);
  uVar4 = (uint)CARRY2(uVar7,uVar8);
  bVar16 = CARRY2(uVar11,uVar4);
  uVar11 = uVar11 + uVar4;
  if (bVar16) {
    uVar11 = (uint)(CONCAT12(1,uVar11) >> 1);
    local_6 = local_6 + 1;
  }
  if ((int)local_6 < 0x4001) {
    uVar7 = uVar7 + uVar8;
    uVar13 = uVar13 + uVar9;
    if (-0x3fff < (int)local_6) goto LAB_2704_022e;
    local_6 = 0xc001;
  }
  else {
    local_6 = 0x4001;
  }
  FUN_2704_2714();
  local_7 = 0;
  uVar3 = 0;
  uVar11 = 0;
  uVar7 = uVar3;
  uVar13 = uVar3;
  goto LAB_2704_022e;
}


