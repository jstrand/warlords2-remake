
/* WARNING (jumptable): Unable to track spacebase fully for stack */

undefined2 __vprinter_body(void)

{
  char *pcVar1;
  uint *puVar2;
  undefined1 *puVar3;
  byte bVar4;
  undefined2 *puVar5;
  uint uVar6;
  int *piVar7;
  char cVar8;
  undefined2 uVar9;
  uint uVar10;
  byte bVar13;
  int iVar11;
  int iVar12;
  int extraout_DX;
  int extraout_DX_00;
  int extraout_DX_01;
  char cVar14;
  int iVar15;
  int unaff_BP;
  char *pcVar16;
  char *pcVar17;
  char *pcVar18;
  int *piVar19;
  undefined1 *puVar20;
  uint *puVar21;
  int *piVar22;
  undefined2 uVar23;
  int iVar24;
  int unaff_SS;
  bool bVar25;
  
  *(int *)(unaff_BP + -4) = unaff_BP + -0x96;
LAB_1000_296a:
  pcVar18 = (char *)*(undefined2 *)(unaff_BP + -4);
LAB_1000_296d:
  uVar23 = (undefined2)((ulong)*(undefined4 *)(unaff_BP + 6) >> 0x10);
  pcVar16 = (char *)(char *)*(undefined4 *)(unaff_BP + 6);
LAB_1000_2970:
  pcVar17 = pcVar16 + 1;
  cVar8 = *pcVar16;
  if (cVar8 == '\0') goto LAB_1000_2d70;
  if (cVar8 == '%') {
    *(int *)(unaff_BP + -0x10) = (int)pcVar17;
    pcVar1 = pcVar17;
    pcVar17 = pcVar16 + 2;
    cVar8 = *pcVar1;
    if (cVar8 != '%') goto code_r0x00012994;
  }
  *pcVar18 = cVar8;
  pcVar18 = pcVar18 + 1;
  pcVar1 = (char *)(unaff_BP + -0x14);
  cVar8 = *pcVar1;
  *pcVar1 = *pcVar1 + -1;
  pcVar16 = pcVar17;
  if (*pcVar1 == '\0' || SBORROW1(cVar8,'\x01') != *pcVar1 < '\0') {
    __vp_flush();
  }
  goto LAB_1000_2970;
code_r0x00012994:
  *(undefined2 *)(unaff_BP + -4) = pcVar18;
  uVar10 = 0;
  *(undefined2 *)(unaff_BP + -0xe) = 0;
  *(undefined2 *)(unaff_BP + -2) = 0x20;
  *(undefined1 *)(unaff_BP + -0xb) = 0;
  *(undefined2 *)(unaff_BP + -8) = 0xffff;
  *(undefined2 *)(unaff_BP + -10) = 0xffff;
  do {
    if ((0x5f < (byte)(cVar8 - 0x20U)) ||
       (bVar4 = *(byte *)((byte)(cVar8 - 0x20U) + 0x3e0f), 0x17 < bVar4))
    goto switchD_1000_29ce_caseD_13;
    cVar14 = bVar4 * '\x02';
    bVar13 = (byte)(uVar10 >> 8);
    iVar24 = unaff_SS;
                    /* WARNING: Switch is manually overridden */
    switch(bVar4) {
    case 0:
      if (bVar13 != 0) goto switchD_1000_29ce_caseD_13;
      if (*(char *)(unaff_BP + -0xb) != '+') {
        *(char *)(unaff_BP + -0xb) = cVar8;
      }
      break;
    case 1:
      if (bVar13 != 0) goto switchD_1000_29ce_caseD_13;
      *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) | 1;
      break;
    case 2:
      iVar24 = *(int *)*(undefined2 *)(unaff_BP + 4);
      *(int *)(unaff_BP + 4) = *(int *)(unaff_BP + 4) + 2;
      if (bVar13 < 2) {
        if (iVar24 < 0) {
          iVar24 = -iVar24;
          *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) | 2;
        }
        *(int *)(unaff_BP + -8) = iVar24;
        uVar10 = 0x300;
      }
      else {
        if (bVar13 != 4) goto switchD_1000_29ce_caseD_13;
        *(int *)(unaff_BP + -10) = iVar24;
        uVar10 = 0x500;
      }
      break;
    case 3:
      if (bVar13 != 0) goto switchD_1000_29ce_caseD_13;
      *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) | 2;
      break;
    case 4:
      if (3 < bVar13) goto switchD_1000_29ce_caseD_13;
      uVar10 = 0x400;
      *(int *)(unaff_BP + -10) = *(int *)(unaff_BP + -10) + 1;
      break;
    case 5:
switchD_1000_29ce_caseD_5:
      if (bVar13 < 3) {
        uVar10 = 0x200;
        LOCK();
        iVar24 = *(int *)(unaff_BP + -8);
        *(int *)(unaff_BP + -8) = (int)(char)(cVar8 + -0x30);
        UNLOCK();
        if (-1 < iVar24) {
          *(int *)(unaff_BP + -8) = *(int *)(unaff_BP + -8) + iVar24 * 10;
        }
      }
      else {
        if (bVar13 != 4) {
switchD_1000_29ce_caseD_13:
          pcVar18 = (char *)*(undefined2 *)(unaff_BP + -0x10);
          uVar23 = *(undefined2 *)(unaff_BP + 8);
          do {
            __vp_putc();
            pcVar1 = pcVar18;
            pcVar18 = pcVar18 + 1;
          } while (*pcVar1 != '\0');
LAB_1000_2d70:
          if (*(char *)(unaff_BP + -0x14) < 'P') {
            __vp_flush();
          }
          if (*(int *)(unaff_BP + -0x16) == 0) {
            uVar23 = *(undefined2 *)(unaff_BP + -0x12);
          }
          else {
            uVar23 = 0xffff;
          }
          return uVar23;
        }
        LOCK();
        iVar24 = *(int *)(unaff_BP + -10);
        *(int *)(unaff_BP + -10) = (int)(char)(cVar8 + -0x30);
        UNLOCK();
        if (iVar24 != 0) {
          *(int *)(unaff_BP + -10) = *(int *)(unaff_BP + -10) + iVar24 * 10;
        }
      }
      break;
    case 6:
      *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) | 0x10;
      goto LAB_1000_2a06;
    case 7:
      *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) | 0x100;
    case 8:
      *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) & 0xffef;
      goto LAB_1000_2a06;
    case 9:
      if (bVar13 != 0) goto switchD_1000_29ce_caseD_5;
      if ((*(uint *)(unaff_BP + -2) & 2) == 0) {
        *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) | 8;
        uVar10 = 0x100;
      }
      break;
    case 10:
      goto switchD_1000_29ce_caseD_a;
    case 0xb:
      uVar23 = CONCAT11(8,cVar14);
      goto LAB_1000_2abb;
    case 0xc:
      uVar23 = CONCAT11(10,cVar14);
      goto LAB_1000_2abf;
    case 0xd:
      uVar23 = CONCAT11(0x10,cVar8 + -0x17);
LAB_1000_2abb:
      *(undefined1 *)(unaff_BP + -0xb) = 0;
LAB_1000_2abf:
      *(char *)(unaff_BP + -5) = cVar8;
      iVar11 = 0;
      *(undefined1 *)(unaff_BP + -6) = 0;
      piVar22 = (int *)*(undefined2 *)(unaff_BP + 4);
      iVar15 = *piVar22;
      goto LAB_1000_2adf;
    case 0xe:
      *(char *)(unaff_BP + -5) = cVar8;
      *(undefined2 *)(unaff_BP + 6) = pcVar17;
      puVar20 = (undefined1 *)(unaff_BP + -0x46);
      puVar5 = (undefined2 *)*(int *)(unaff_BP + 4);
      uVar23 = *puVar5;
      *(undefined2 *)(unaff_BP + 4) = puVar5 + 1;
      if ((*(uint *)(unaff_BP + -2) & 0x20) != 0) {
        *(undefined2 *)(unaff_BP + 4) = puVar5 + 2;
        __put_hex_word(uVar23);
        puVar3 = puVar20;
        puVar20 = (undefined1 *)(unaff_BP + -0x45);
        *puVar3 = 0x3a;
      }
      __put_hex_word();
      *puVar20 = 0;
      *(undefined1 *)(unaff_BP + -6) = 0;
      *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) & 0xfffb;
      puVar21 = (uint *)(unaff_BP + -0x46);
      uVar10 = (int)puVar20 - (int)puVar21;
      goto LAB_1000_2c20;
    case 0xf:
      *(undefined2 *)(unaff_BP + 6) = pcVar17;
      *(char *)(unaff_BP + -5) = cVar8;
      uVar23 = *(undefined2 *)(unaff_BP + 4);
      uVar10 = *(uint *)(unaff_BP + -10);
      if ((int)uVar10 < 0) {
        uVar10 = 6;
      }
      uVar6 = *(uint *)(unaff_BP + -2);
      if ((*(uint *)(unaff_BP + -2) & 0x100) == 0) {
        *(int *)(unaff_BP + 4) = *(int *)(unaff_BP + 4) + 8;
        uVar9 = 6;
      }
      else {
        uVar9 = 8;
        *(int *)(unaff_BP + 4) = *(int *)(unaff_BP + 4) + 10;
      }
      __realcvt(uVar9,uVar6 & 1,cVar8,unaff_BP + -0x45,unaff_SS,uVar10,uVar23,unaff_SS);
      puVar21 = (uint *)(unaff_BP + -0x45);
      goto LAB_1000_2c20;
    case 0x10:
      *(undefined2 *)(unaff_BP + 6) = pcVar17;
      *(char *)(unaff_BP + -5) = cVar8;
      uVar10 = *(uint *)*(undefined2 *)(unaff_BP + 4);
      *(int *)(unaff_BP + 4) = *(int *)(unaff_BP + 4) + 2;
      puVar21 = (uint *)(unaff_BP + -0x45);
      *puVar21 = uVar10 & 0xff;
      uVar10 = 1;
      goto LAB_1000_2c63;
    case 0x11:
      *(undefined2 *)(unaff_BP + 6) = pcVar17;
      *(char *)(unaff_BP + -5) = cVar8;
      if ((*(uint *)(unaff_BP + -2) & 0x20) == 0) {
        puVar21 = *(uint **)(undefined4 *)*(undefined2 *)(unaff_BP + 4);
        *(int *)(unaff_BP + 4) = *(int *)(unaff_BP + 4) + 2;
        iVar24 = 0x4125;
        bVar25 = puVar21 == (uint *)0x0;
      }
      else {
        puVar2 = (uint *)*(undefined4 *)*(undefined2 *)(unaff_BP + 4);
        iVar24 = (int)((ulong)puVar2 >> 0x10);
        puVar21 = (uint *)puVar2;
        *(int *)(unaff_BP + 4) = *(int *)(unaff_BP + 4) + 4;
        bVar25 = iVar24 == 0 && puVar21 == (uint *)0x0;
      }
      if (bVar25) {
        iVar24 = 0x4125;
        puVar21 = (uint *)&DAT_4125_3e08;
      }
      __vp_strlen();
      if (*(uint *)(unaff_BP + -10) < uVar10) {
        uVar10 = *(uint *)(unaff_BP + -10);
      }
      goto LAB_1000_2c63;
    case 0x12:
      *(undefined2 *)(unaff_BP + 6) = pcVar17;
      if ((*(uint *)(unaff_BP + -2) & 0x20) == 0) {
        piVar22 = *(int **)(undefined4 *)*(undefined2 *)(unaff_BP + 4);
        *(int *)(unaff_BP + 4) = *(int *)(unaff_BP + 4) + 2;
        uVar23 = 0x4125;
      }
      else {
        piVar7 = (int *)*(undefined4 *)*(undefined2 *)(unaff_BP + 4);
        uVar23 = (undefined2)((ulong)piVar7 >> 0x10);
        piVar22 = (int *)piVar7;
        *(int *)(unaff_BP + 4) = *(int *)(unaff_BP + 4) + 4;
      }
      *piVar22 = (uint)(byte)(0x50 - *(char *)(unaff_BP + -0x14)) + *(int *)(unaff_BP + -0x12);
      if ((*(uint *)(unaff_BP + -2) & 0x10) != 0) {
        piVar22[1] = 0;
      }
      goto LAB_1000_296a;
    default:
      goto switchD_1000_29ce_caseD_13;
    case 0x16:
      *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) & 0xffdf;
      goto LAB_1000_2a06;
    case 0x17:
      *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) | 0x20;
LAB_1000_2a06:
      uVar10 = 0x500;
    }
    pcVar1 = pcVar17;
    pcVar17 = pcVar17 + 1;
    cVar8 = *pcVar1;
  } while( true );
switchD_1000_29ce_caseD_a:
  uVar23 = CONCAT11(10,cVar14);
  *(undefined1 *)(unaff_BP + -6) = 1;
  *(char *)(unaff_BP + -5) = cVar8;
  piVar22 = (int *)*(undefined2 *)(unaff_BP + 4);
  iVar15 = *piVar22;
  iVar11 = iVar15 >> 0xf;
LAB_1000_2adf:
  piVar19 = piVar22 + 1;
  *(undefined2 *)(unaff_BP + 6) = pcVar17;
  if ((*(uint *)(unaff_BP + -2) & 0x10) != 0) {
    iVar11 = *piVar19;
    piVar19 = piVar22 + 2;
  }
  *(undefined2 *)(unaff_BP + 4) = piVar19;
  puVar21 = (uint *)(unaff_BP + -0x45);
  if ((iVar15 == 0) && (iVar11 == 0)) {
    if (*(int *)(unaff_BP + -10) != 0) goto LAB_1000_2b10;
    *(char *)puVar21 = '\0';
  }
  else {
    *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) | 4;
LAB_1000_2b10:
    __longtoa(uVar23,*(undefined1 *)(unaff_BP + -6),(char)((uint)uVar23 >> 8),puVar21,unaff_SS,
              iVar15,iVar11);
  }
  if (*(int *)(unaff_BP + -10) < 0) {
LAB_1000_2c20:
    if (((*(uint *)(unaff_BP + -2) & 8) != 0) && (0 < *(int *)(unaff_BP + -8))) goto LAB_1000_2c2e;
  }
  else {
LAB_1000_2c2e:
    __vp_strlen();
    if ((char)*puVar21 == '-') {
      uVar10 = uVar10 - 1;
    }
    if (extraout_DX - uVar10 != 0 && (int)uVar10 <= extraout_DX) {
      *(int *)(unaff_BP + -0xe) = extraout_DX - uVar10;
    }
  }
  if ((char)*puVar21 != '-') {
    if (*(char *)(unaff_BP + -0xb) == '\0') goto LAB_1000_2c60;
    puVar21 = (uint *)((int)puVar21 + -1);
    *(char *)puVar21 = *(char *)(unaff_BP + -0xb);
  }
  if ((0 < *(int *)(unaff_BP + -0xe)) && (uVar10 = *(uint *)(unaff_BP + -10), (int)uVar10 < 0)) {
    *(int *)(unaff_BP + -0xe) = *(int *)(unaff_BP + -0xe) + -1;
  }
LAB_1000_2c60:
  __vp_strlen();
LAB_1000_2c63:
  pcVar18 = (char *)*(undefined2 *)(unaff_BP + -4);
  iVar15 = *(int *)(unaff_BP + -8);
  if ((*(uint *)(unaff_BP + -2) & 5) == 5) {
    cVar8 = *(char *)(unaff_BP + -5);
    if (cVar8 == 'o') {
      if (*(int *)(unaff_BP + -0xe) < 1) {
        *(undefined2 *)(unaff_BP + -0xe) = 1;
      }
    }
    else if ((cVar8 == 'x') || (cVar8 == 'X')) {
      *(uint *)(unaff_BP + -2) = *(uint *)(unaff_BP + -2) | 0x40;
      iVar15 = iVar15 + -2;
      piVar7 = (int *)(unaff_BP + -0xe);
      iVar11 = *piVar7;
      *piVar7 = *piVar7 + -2;
      if (SBORROW2(iVar11,2) != *piVar7 < 0) {
        *(undefined2 *)(unaff_BP + -0xe) = 0;
      }
    }
  }
  iVar11 = uVar10 + *(int *)(unaff_BP + -0xe);
  if ((*(uint *)(unaff_BP + -2) & 2) == 0) {
    for (; iVar11 < iVar15; iVar15 = iVar15 + -1) {
      __vp_putc();
    }
  }
  if ((*(uint *)(unaff_BP + -2) & 0x40) != 0) {
    __vp_putc();
    __vp_putc();
  }
  iVar12 = *(int *)(unaff_BP + -0xe);
  if (0 < iVar12) {
    iVar11 = iVar11 - iVar12;
    iVar15 = iVar15 - iVar12;
    cVar8 = (char)*puVar21;
    if (((cVar8 == '-') || (cVar8 == ' ')) || (cVar8 == '+')) {
      puVar21 = (uint *)((int)puVar21 + 1);
      __vp_putc();
      iVar11 = iVar11 + -1;
      iVar15 = iVar15 + -1;
      iVar12 = extraout_DX_00;
    }
    for (; iVar12 != 0; iVar12 = iVar12 + -1) {
      __vp_putc();
      iVar11 = extraout_DX_01;
    }
  }
  if (iVar11 != 0) {
    iVar15 = iVar15 - iVar11;
    do {
      puVar2 = puVar21;
      puVar21 = (uint *)((int)puVar21 + 1);
      *pcVar18 = (char)*puVar2;
      pcVar18 = pcVar18 + 1;
      pcVar1 = (char *)(unaff_BP + -0x14);
      cVar8 = *pcVar1;
      *pcVar1 = *pcVar1 + -1;
      if (*pcVar1 == '\0' || SBORROW1(cVar8,'\x01') != *pcVar1 < '\0') {
        __vp_flush();
      }
      iVar11 = iVar11 + -1;
    } while (iVar11 != 0);
  }
  if (0 < iVar15) {
    do {
      __vp_putc();
      iVar15 = iVar15 + -1;
    } while (iVar15 != 0);
  }
  goto LAB_1000_296d;
}


