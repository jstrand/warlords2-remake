
undefined4 __cdecl16near
__scantol(code *param_1,code *param_2,undefined2 param_3,undefined2 param_4,uint param_5,int param_6
         ,int *param_7,undefined2 *param_8)

{
  int iVar1;
  long lVar2;
  int iVar3;
  char cVar4;
  byte bVar5;
  uint uVar6;
  uint uVar7;
  byte bVar8;
  uint uVar9;
  uint uVar10;
  byte bVar11;
  uint uVar12;
  uint uVar13;
  undefined1 uVar14;
  bool bVar15;
  undefined2 local_8;
  int local_6;
  char local_3;
  
  local_3 = '\0';
  local_6 = 0;
  local_8 = 1;
  iVar1 = local_6;
  do {
    local_6 = iVar1;
    iVar1 = local_6 + 1;
    uVar6 = (*param_1)(param_3,param_4);
    if ((int)uVar6 < 0) goto LAB_1000_25e8;
    cVar4 = (char)uVar6;
    uVar6 = (uint)cVar4;
  } while (((uVar6 & 0x80) == 0) && ((*(byte *)(uVar6 + 0x3a5f) & 1) != 0));
  if (param_6 < 1) {
LAB_1000_25ef:
    local_6 = iVar1;
    local_8 = 0;
  }
  else {
    if (cVar4 != '+') {
      iVar3 = param_6 + -1;
      if (cVar4 == '-') {
        local_3 = '\x01';
        goto LAB_1000_2597;
      }
LAB_1000_25ae:
      local_6 = iVar1;
      param_6 = iVar3;
      uVar12 = 0;
      uVar13 = 0;
      bVar5 = (byte)uVar6;
      iVar1 = local_6;
      if (param_5 == 0) {
        param_5 = 10;
        if (bVar5 != 0x30) {
LAB_1000_263d:
          uVar14 = bVar5 < 0x30;
          __scan_digit();
          uVar12 = uVar6;
          if (!(bool)uVar14) goto LAB_1000_2654;
          goto LAB_1000_25ef;
        }
        iVar1 = param_6 + -1;
        if (0 < param_6) {
          local_6 = local_6 + 1;
          uVar6 = (*param_1)(param_3,param_4);
          param_5 = 8;
          bVar5 = (byte)uVar6;
          uVar14 = bVar5 < 0x78;
          param_6 = iVar1;
          if ((bVar5 == 0x78) || (uVar14 = bVar5 < 0x58, bVar5 == 0x58)) {
            param_5 = 0x10;
            goto LAB_1000_2654;
          }
LAB_1000_2667:
          while (__scan_digit(), !(bool)uVar14) {
            uVar9 = (uint)((ulong)uVar12 * (ulong)param_5 >> 0x10);
            uVar7 = (uint)((ulong)uVar12 * (ulong)param_5);
            uVar12 = uVar6 + uVar7;
            uVar6 = (uint)CARRY2(uVar6,uVar7);
            uVar7 = uVar13 + uVar9;
            uVar14 = CARRY2(uVar13,uVar9) || CARRY2(uVar7,uVar6);
            uVar13 = uVar7 + uVar6;
            if (uVar13 != 0) goto LAB_1000_2684;
LAB_1000_2654:
            if (param_6 < 1) goto LAB_1000_26b1;
            local_6 = local_6 + 1;
            uVar6 = (*param_1)(param_3,param_4);
            param_6 = param_6 + -1;
          }
LAB_1000_26a1:
          (*param_2)(uVar6,param_3,param_4);
          local_6 = local_6 + -1;
        }
      }
      else {
        if ((0x24 < param_5) || (bVar8 = (byte)param_5, bVar8 < 2)) goto LAB_1000_25ef;
        if (bVar5 != 0x30) goto LAB_1000_263d;
        uVar14 = bVar8 < 0x10;
        if (bVar8 != 0x10) goto LAB_1000_2654;
        iVar1 = param_6 + -1;
        if (0 < param_6) {
          local_6 = local_6 + 1;
          uVar6 = (*param_1)(param_3,param_4);
          bVar5 = (byte)uVar6;
          uVar14 = bVar5 < 0x78;
          param_6 = iVar1;
          if (bVar5 == 0x78) goto LAB_1000_2654;
          uVar14 = bVar5 < 0x58;
          if (bVar5 == 0x58) goto LAB_1000_2654;
          goto LAB_1000_2667;
        }
      }
LAB_1000_26b1:
      if (local_3 != '\0') {
        bVar15 = uVar12 != 0;
        uVar12 = -uVar12;
        uVar13 = -(uint)bVar15 - uVar13;
      }
      goto LAB_1000_26c1;
    }
LAB_1000_2597:
    if (param_6 + -1 < 1) goto LAB_1000_25ef;
    uVar6 = (*param_1)(param_3,param_4);
    iVar3 = param_6 + -2;
    iVar1 = local_6 + 2;
    if (-1 < (int)uVar6) goto LAB_1000_25ae;
LAB_1000_25e8:
    local_6 = iVar1;
    local_8 = 0xffff;
  }
  (*param_2)(uVar6,param_3,param_4);
  local_6 = local_6 + -1;
  uVar12 = 0;
  uVar13 = 0;
LAB_1000_26c1:
  *param_7 = *param_7 + local_6;
  *param_8 = local_8;
  return CONCAT22(uVar13,uVar12);
  while( true ) {
    local_6 = local_6 + 1;
    uVar6 = (*param_1)(param_3,param_4);
    uVar7 = param_5;
    __scan_digit();
    if ((bool)uVar14) goto LAB_1000_26a1;
    uVar10 = (uint)((ulong)uVar12 * (ulong)uVar7 >> 0x10);
    uVar9 = (uint)((ulong)uVar12 * (ulong)uVar7);
    lVar2 = (ulong)uVar13 * (ulong)uVar7;
    uVar12 = uVar6 + uVar9;
    uVar6 = (uint)CARRY2(uVar6,uVar9);
    uVar13 = (uint)lVar2 + uVar10;
    bVar15 = CARRY2((uint)lVar2,uVar10) || CARRY2(uVar13,uVar6);
    uVar13 = uVar13 + uVar6;
    bVar8 = (byte)((ulong)lVar2 >> 0x10);
    bVar11 = (byte)((ulong)lVar2 >> 0x18);
    bVar5 = bVar8 + bVar11;
    uVar14 = CARRY1(bVar8,bVar11) || CARRY1(bVar5,bVar15);
    param_6 = param_6 + -1;
    if ((byte)(bVar5 + bVar15) != '\0') break;
LAB_1000_2684:
    if (param_6 < 1) goto LAB_1000_26b1;
  }
  uVar12 = CONCAT11((local_3 != '\0') + -1,local_3 + -1);
  uVar13 = (local_3 != '\0') + 0x7fff;
  local_8 = 2;
  goto LAB_1000_26c1;
}


