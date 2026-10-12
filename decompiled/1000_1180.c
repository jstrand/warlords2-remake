
void __realcvt(undefined2 param_1,char param_2,byte param_3,byte *param_4,uint param_5)

{
  byte *pbVar1;
  ulong uVar2;
  int iVar3;
  undefined2 uVar4;
  uint uVar5;
  byte bVar6;
  int iVar7;
  int iVar8;
  byte *pbVar9;
  byte *pbVar10;
  byte *pbVar11;
  byte *pbVar12;
  undefined2 uVar13;
  undefined2 unaff_SS;
  byte local_32 [2];
  byte abStack_30 [42];
  uint local_6;
  int local_4;
  
  if (0x28 < param_5) {
    param_5 = 0x28;
  }
  local_6 = param_5;
  if ((param_3 & 0xdf) == 0x46) {
    if (0x7fff < param_5) {
      param_5 = 0;
    }
LAB_1000_11b9:
    if ((param_3 & 0xdf) == 0x45) {
      param_5 = param_5 + 1;
    }
  }
  else if (0 < (int)param_5) goto LAB_1000_11b9;
  pbVar10 = local_32;
  iVar3 = __xcvt(param_1,pbVar10);
  uVar13 = (undefined2)((ulong)param_4 >> 0x10);
  pbVar11 = (byte *)param_4;
  if (iVar3 == 0x7fff) {
    uVar4 = 0x492b;
    if (local_4 != 0) {
      uVar4 = 0x492d;
    }
    *(undefined2 *)param_4 = uVar4;
    pbVar9 = pbVar11 + 4;
    (pbVar11 + 2)[0] = 0x4e;
    (pbVar11 + 2)[1] = 0x46;
    goto LAB_1000_1309;
  }
  if (iVar3 == 0x7ffe) {
    uVar4 = 0x4e2b;
    if (local_4 != 0) {
      uVar4 = 0x4e2d;
    }
    *(undefined2 *)param_4 = uVar4;
    pbVar9 = pbVar11 + 4;
    *(uint *)(pbVar11 + 2) = CONCAT11((char)((uint)uVar4 >> 8),0x41);
    goto LAB_1000_1309;
  }
  pbVar12 = pbVar11;
  if ((char)local_4 != '\0') {
    pbVar12 = pbVar11 + 1;
    *param_4 = 0x2d;
  }
  if ((param_3 & 0x5f) == 0x46) {
LAB_1000_123c:
    if (iVar3 < 0x29) {
      if (iVar3 < 1) {
        pbVar1 = pbVar12;
        pbVar12 = pbVar12 + 2;
        pbVar1[0] = 0x30;
        pbVar1[1] = 0x2e;
        iVar7 = 1;
        iVar8 = iVar3;
        if (iVar3 != 0) {
          do {
            pbVar1 = pbVar12;
            pbVar12 = pbVar12 + 1;
            *pbVar1 = 0x30;
            iVar8 = iVar8 + 1;
            iVar3 = 0;
          } while (iVar8 != 0);
          goto LAB_1000_1254;
        }
      }
      else {
LAB_1000_1254:
        iVar7 = 0;
      }
      while( true ) {
        pbVar9 = pbVar12;
        pbVar1 = pbVar10;
        pbVar10 = pbVar10 + 1;
        if (*pbVar1 == 0) break;
        *pbVar9 = *pbVar1;
        iVar3 = iVar3 + -1;
        pbVar12 = pbVar9 + 1;
        if (iVar3 == 0) {
          pbVar9[1] = 0x2e;
          iVar7 = iVar7 + 1;
          pbVar12 = pbVar9 + 2;
        }
      }
      if (iVar7 + local_6 < param_5) {
        iVar8 = param_5 - (iVar7 + local_6);
        iVar3 = iVar3 + iVar8;
        for (; iVar8 != 0; iVar8 = iVar8 + -1) {
          pbVar1 = pbVar9;
          pbVar9 = pbVar9 + 1;
          *pbVar1 = 0x30;
        }
        iVar3 = iVar3 + -1;
        if (iVar3 != 0) goto LAB_1000_127e;
      }
      else {
LAB_1000_127e:
        if ((iVar3 != 1) && (param_2 == '\0')) {
          __trim_zeros();
        }
      }
      if (pbVar9 == pbVar11) {
        pbVar1 = pbVar9;
        pbVar9 = pbVar9 + 1;
        *pbVar1 = 0x30;
      }
      goto LAB_1000_1309;
    }
  }
  else if (((param_3 & 0x5f) != 0x45) && (-4 < iVar3)) {
    uVar5 = param_5;
    if (param_5 == 0) {
      uVar5 = 1;
    }
    if (iVar3 <= (int)uVar5) goto LAB_1000_123c;
  }
  pbVar10 = abStack_30;
  pbVar11 = pbVar12 + 1;
  *pbVar12 = local_32[0];
  bVar6 = local_32[1];
  if (bVar6 == 0) {
    if (param_2 != '\0') {
      pbVar1 = pbVar11;
      pbVar11 = pbVar12 + 2;
      *pbVar1 = 0x2e;
    }
  }
  else {
    pbVar1 = pbVar11;
    pbVar11 = pbVar12 + 2;
    *pbVar1 = 0x2e;
    do {
      pbVar1 = pbVar11;
      pbVar11 = pbVar11 + 1;
      *pbVar1 = bVar6;
      pbVar1 = pbVar10;
      pbVar10 = pbVar10 + 1;
      bVar6 = *pbVar1;
    } while (bVar6 != 0);
    if (param_2 == '\0') {
      __trim_zeros();
    }
  }
  *pbVar11 = param_3 & 0x20 | 0x45;
  bVar6 = 0x2b;
  uVar5 = iVar3 - 1;
  if (iVar3 < 1) {
    bVar6 = 0x2d;
    uVar5 = -uVar5;
  }
  pbVar10 = pbVar11 + 2;
  pbVar11[1] = bVar6;
  if (99 < uVar5) {
    if (999 < uVar5) {
      uVar2 = (ulong)(int)uVar5;
      uVar5 = (uint)(uVar2 % 1000);
      pbVar1 = pbVar10;
      pbVar10 = pbVar11 + 3;
      *pbVar1 = (char)(uVar2 / 1000) + 0x30;
    }
    pbVar1 = pbVar10;
    pbVar10 = pbVar10 + 1;
    *pbVar1 = (char)(uVar5 / 100) + 0x30;
    uVar5 = (uint)(char)(uVar5 % 100);
  }
  pbVar9 = pbVar10 + 2;
  *(int *)pbVar10 = CONCAT11((char)(uVar5 % 10),(char)(uVar5 / 10)) + 0x3030;
LAB_1000_1309:
  *pbVar9 = 0;
  return;
}


