
byte FUN_2704_02bd(uint *param_1,undefined2 *param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  ulong uVar4;
  ulong uVar5;
  uint uVar6;
  uint uVar7;
  byte bVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  undefined2 unaff_ES;
  bool bVar12;
  bool bVar13;
  long lVar14;
  
  bVar8 = *(byte *)(param_3 + 10) ^ *(byte *)(param_2 + 5);
  uVar11 = param_2[4];
  uVar9 = 0xc001;
  uVar6 = *(uint *)(param_3 + 8);
  bVar12 = uVar6 < 0x4001;
  if ((int)uVar6 < 0x4001) {
    bVar12 = uVar11 < 0x4001;
    if ((int)uVar11 < 0x4001) {
      if ((-0x3fff < (int)uVar6) && (-0x3fff < (int)uVar11)) {
        uVar6 = uVar6 + uVar11;
        if ((int)(uVar6 - 1) < 0x4001) {
          if (-0x3fff < (int)uVar6) {
            lVar14 = FUN_2704_0353(*param_2,param_2[1],param_2[2],param_2[3]);
            if (-1 < lVar14) {
              uVar6 = uVar6 - 1;
              bVar13 = param_3 < 0;
              param_3 = param_3 << 1;
              uVar4 = (ulong)CONCAT12(bVar13,(int)lVar14) << 1;
              bVar12 = (uVar4 & 0x10000) != 0;
              uVar5 = (ulong)CONCAT12(bVar12,uVar11) << 1;
              uVar11 = (uint)uVar5 | (uint)bVar12;
              bVar12 = (uVar5 & 0x10000) != 0;
              uVar5 = (ulong)CONCAT12(bVar12,uVar9) << 1;
              uVar9 = (uint)uVar5 | (uint)bVar12;
              lVar14 = CONCAT22((int)((ulong)lVar14 >> 0x10) << 1 | (uint)((uVar5 & 0x10000) != 0),
                                (uint)uVar4 | (uint)bVar13);
            }
            uVar10 = (uint)((ulong)lVar14 >> 0x10);
            uVar7 = (uint)lVar14 + (uint)(param_3 < 0);
            if (CARRY2((uint)lVar14,(uint)(param_3 < 0))) {
              bVar12 = CARRY2(uVar11,uVar7);
              uVar2 = uVar11 + uVar7;
              uVar11 = uVar2 + 1;
              uVar2 = (uint)(bVar12 || 0xfffe < uVar2);
              bVar12 = CARRY2(uVar9,uVar7);
              uVar3 = uVar9 + uVar7;
              uVar9 = uVar3 + uVar2;
              uVar2 = (uint)(bVar12 || CARRY2(uVar3,uVar2));
              bVar12 = CARRY2(uVar10,uVar7);
              uVar3 = uVar10 + uVar7;
              uVar10 = uVar3 + uVar2;
              if (bVar12 || CARRY2(uVar3,uVar2)) {
                uVar10 = (uint)(CONCAT12(bVar12 || CARRY2(uVar3,uVar2),uVar10) >> 1);
                uVar6 = uVar6 + 1;
              }
            }
            *param_1 = uVar7;
            param_1[1] = uVar11;
            param_1[2] = uVar9;
            param_1[3] = uVar10;
            param_1[4] = uVar6;
            *(byte *)(param_1 + 5) = bVar8;
            return bVar8;
          }
          FUN_2704_2714();
        }
        else {
          FUN_2704_2714();
        }
      }
      goto LAB_2704_026c;
    }
    FUN_2704_0012();
    if (bVar12) {
      return bVar8;
    }
    iVar1 = *(int *)(param_3 + 8);
  }
  else {
    FUN_2704_0012();
    if (bVar12) {
      return bVar8;
    }
    iVar1 = param_2[4];
  }
  if (iVar1 < -0x3ffe) {
    FUN_2704_0e53();
    return bVar8;
  }
LAB_2704_026c:
  FUN_2704_0e3d();
  return bVar8;
}


