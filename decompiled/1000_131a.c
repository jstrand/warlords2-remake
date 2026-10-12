
double * __cdecl16near
__scantod(code *param_1,code *param_2,undefined2 param_3,undefined2 param_4,int param_5,int *param_6
         ,undefined2 *param_7)

{
  int iVar1;
  uint uVar2;
  long lVar3;
  long lVar4;
  bool bVar5;
  bool bVar6;
  bool bVar7;
  unkuint10 Var8;
  longdouble lVar9;
  int iVar10;
  int iVar11;
  byte bVar12;
  char cVar13;
  double *__return_storage_ptr__;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  uint uVar17;
  uint uVar18;
  uint uVar19;
  uint uVar20;
  uint uVar21;
  uint uVar22;
  int iVar23;
  uint local_1a;
  undefined6 uVar24;
  undefined2 uVar25;
  undefined2 local_10;
  int local_e;
  char local_b;
  
  bVar5 = true;
  bVar6 = false;
  bVar7 = false;
  local_b = '\0';
  local_e = 0;
  local_10 = 1;
  _local_1a = (longdouble)0;
  uVar21 = 0x8000;
  uVar22 = 0xfffe;
  do {
    local_e = local_e + 1;
    __return_storage_ptr__ = (double *)(*param_1)(param_3,param_4);
    if ((int)__return_storage_ptr__ < 0) {
      local_10 = 0xffff;
      goto LAB_1000_1459;
    }
    cVar13 = (char)__return_storage_ptr__;
    __return_storage_ptr__ = (double *)(int)cVar13;
  } while ((((uint)__return_storage_ptr__ & 0x80) == 0) &&
          ((*(byte *)((int)__return_storage_ptr__ + 0x3a5f) & 1) != 0));
  iVar23 = param_5 + -1;
  if (0 < param_5) {
    if ((cVar13 != '+') && (uVar15 = uVar22, iVar1 = local_e, cVar13 != '-')) goto LAB_1000_13b5;
    bVar6 = true;
    param_5 = iVar23;
    while (iVar23 = param_5 + -1, 0 < param_5) {
      iVar1 = local_e + 1;
      __return_storage_ptr__ = (double *)(*param_1)(param_3,param_4);
      uVar15 = uVar22;
      if ((bVar5) && (bVar6)) {
        if ((char)__return_storage_ptr__ == 'I') {
          __return_storage_ptr__ = (double *)(*param_1)(param_3,param_4);
          iVar1 = param_5 + -2;
          iVar10 = iVar1;
          iVar11 = local_e + 2;
          if ((iVar23 < 1) || ((char)__return_storage_ptr__ != 'N')) goto LAB_1000_1454;
          local_e = local_e + 3;
          __return_storage_ptr__ = (double *)(*param_1)(param_3,param_4);
          iVar10 = param_5 + -3;
          iVar11 = local_e;
          if ((iVar1 < 1) || ((char)__return_storage_ptr__ != 'F')) goto LAB_1000_1454;
          goto LAB_1000_157c;
        }
        if ((char)__return_storage_ptr__ == 'N') {
          __return_storage_ptr__ = (double *)(*param_1)(param_3,param_4);
          iVar1 = param_5 + -2;
          iVar10 = iVar1;
          iVar11 = local_e + 2;
          if ((iVar23 < 1) || ((char)__return_storage_ptr__ != 'A')) goto LAB_1000_1454;
          local_e = local_e + 3;
          __return_storage_ptr__ = (double *)(*param_1)(param_3,param_4);
          iVar10 = param_5 + -3;
          iVar11 = local_e;
          if ((iVar1 < 1) || ((char)__return_storage_ptr__ != 'N')) goto LAB_1000_1454;
          goto LAB_1000_157c;
        }
      }
LAB_1000_13b5:
      local_e = iVar1;
      param_5 = iVar23;
      lVar9 = _local_1a;
      bVar5 = false;
      bVar12 = (byte)__return_storage_ptr__;
      uVar22 = uVar15;
      if (bVar12 == 0x2e) {
        if (uVar21 != 0x8000) goto LAB_1000_1475;
        uVar21 = 0;
        if (0 < (int)uVar15) {
          uVar21 = uVar15;
        }
      }
      else {
        if ((0x39 < bVar12) || (bVar12 < 0x30)) goto LAB_1000_1475;
        cVar13 = bVar12 - 0x30;
        uVar14 = (uint)cVar13;
        uVar22 = uVar15 + 1;
        if (uVar22 == 0 || (int)uVar15 < -1) {
          _local_1a = (longdouble)CONCAT91(stack0xffe7,cVar13);
          uVar22 = 1;
          if ((cVar13 == '\0') && (uVar22 = 0xffff, uVar21 != 0x8000)) {
            uVar21 = uVar21 - 1;
          }
        }
        else if (uVar22 < 6) {
          uVar15 = (uint)((ulong)local_1a * 10);
          Var8 = (unkuint10)_local_1a >> 0x20;
          uVar24 = (undefined6)Var8;
          _local_1a = (ulong)CONCAT12((char)((ulong)local_1a * 10 >> 0x10) + CARRY2(uVar15,uVar14),
                                      uVar15 + uVar14);
        }
        else if (uVar22 < 0x13) {
          Var8 = (unkuint10)_local_1a >> 0x30;
          iVar23 = (int)Var8 * 10;
          uVar25 = (undefined2)((unkuint10)lVar9 >> 0x40);
          lVar3 = (ulong)(uint)((unkuint10)lVar9 >> 0x20) * 10;
          uVar16 = (uint)lVar3;
          _local_1a = SUB104(lVar9,0);
          lVar4 = (_local_1a >> 0x10) * 10;
          uVar19 = (uint)((ulong)lVar4 >> 0x10);
          uVar17 = (uint)lVar4;
          lVar4 = (_local_1a & 0xffff) * 10;
          uVar20 = (uint)((ulong)lVar4 >> 0x10);
          uVar18 = (uint)lVar4;
          uVar15 = (uint)CARRY2(uVar18,uVar14);
          uVar2 = uVar17 + uVar20;
          _local_1a = CONCAT22(uVar2 + uVar15,uVar18 + uVar14);
          uVar15 = (uint)(CARRY2(uVar17,uVar20) || CARRY2(uVar2,uVar15));
          uVar14 = uVar16 + uVar19;
          _local_1a = CONCAT26(iVar23 + (int)((ulong)lVar3 >> 0x10) +
                               (uint)(CARRY2(uVar16,uVar19) || CARRY2(uVar14,uVar15)),
                               CONCAT24(uVar14 + uVar15,_local_1a));
        }
      }
    }
  }
  param_5 = iVar23;
  __return_storage_ptr__ = (double *)0x65;
LAB_1000_1475:
  iVar10 = param_5;
  iVar11 = local_e;
  if (uVar22 == 0xfffe) {
LAB_1000_1454:
    local_e = iVar11;
    param_5 = iVar10;
    local_10 = 0;
LAB_1000_1459:
    if (-1 < param_5) {
      __return_storage_ptr__ = (double *)(*param_2)(__return_storage_ptr__,param_3,param_4);
      local_e = local_e + -1;
    }
  }
  else {
    if (uVar21 == 0x8000) {
      uVar21 = uVar22;
    }
    iVar23 = 0;
    if (((char)__return_storage_ptr__ == 'E') || ((char)__return_storage_ptr__ == 'e')) {
      iVar1 = param_5 + -1;
      if (0 < param_5) {
        local_e = local_e + 1;
        __return_storage_ptr__ = (double *)(*param_1)(param_3,param_4);
        param_5 = iVar1;
        if ((char)__return_storage_ptr__ != '+') {
          if ((char)__return_storage_ptr__ != '-') goto LAB_1000_14c3;
          bVar7 = true;
        }
        while (0 < param_5) {
          local_e = local_e + 1;
          __return_storage_ptr__ = (double *)(*param_1)(param_3,param_4);
          param_5 = param_5 + -1;
LAB_1000_14c3:
          bVar12 = (byte)__return_storage_ptr__;
          if ((0x39 < bVar12) || (bVar12 < 0x30)) goto LAB_1000_14e4;
          __return_storage_ptr__ = (double *)(iVar23 * 10);
          iVar23 = (int)(char)(bVar12 - 0x30) + (int)__return_storage_ptr__;
          if (0x1344 < iVar23) {
            iVar23 = 0;
            local_b = '\x01';
          }
        }
      }
    }
    else {
LAB_1000_14e4:
      __return_storage_ptr__ = (double *)(*param_2)(__return_storage_ptr__,param_3,param_4);
      local_e = local_e + -1;
    }
    if (bVar7) {
      iVar23 = -iVar23;
      local_b = -local_b;
    }
    if (-1 < (int)uVar22) {
      __return_storage_ptr__ = (double *)(uVar21 + iVar23);
      if (local_b == '\x01') {
        __return_storage_ptr__ = (double *)0xffff;
      }
      else if (local_b != -1) {
        if (0x12 < uVar22) {
          uVar22 = 0x12;
        }
        __return_storage_ptr__ = (double *)(iVar23 + (uVar21 - uVar22));
        if (__return_storage_ptr__ != (double *)0x0) {
          if ((int)__return_storage_ptr__ < 0) {
            __return_storage_ptr__ = (double *)-(int)__return_storage_ptr__;
          }
          Var8 = (unkuint10)_local_1a >> 0x10;
          __pow10(__return_storage_ptr__,(double)Var8);
        }
        goto LAB_1000_157c;
      }
      local_10 = 2;
    }
  }
LAB_1000_157c:
  *param_6 = *param_6 + local_e;
  *param_7 = local_10;
  return __return_storage_ptr__;
}


