
/* WARNING (jumptable): Unable to track spacebase fully for stack */

undefined2 __cdecl16near __scanner_body(void)

{
  byte *pbVar1;
  char *pcVar2;
  undefined2 *puVar3;
  char cVar4;
  int iVar5;
  int iVar6;
  char *pcVar7;
  uint uVar8;
  undefined2 uVar9;
  byte bVar10;
  uint uVar11;
  int unaff_BP;
  undefined2 uVar12;
  byte *pbVar13;
  byte *pbVar14;
  undefined2 *puVar15;
  byte *pbVar16;
  undefined2 unaff_SS;
  bool bVar17;
  char cVar18;
  undefined4 uVar19;
  
LAB_1000_2061:
  pbVar14 = (byte *)*(undefined2 *)(unaff_BP + 0xc);
LAB_1000_2064:
  pbVar1 = pbVar14;
  pbVar14 = pbVar14 + 1;
  bVar10 = *pbVar1;
  if (bVar10 == 0) goto switchD_1000_2105_caseD_0;
  if (bVar10 != 0x25) goto LAB_1000_2071;
  *(undefined2 *)(unaff_BP + -10) = 0xffff;
  uVar9 = *(undefined2 *)(unaff_BP + 0xe);
  *(undefined1 *)(unaff_BP + -1) = 0x20;
  pbVar16 = pbVar14;
LAB_1000_20ea:
  pbVar14 = pbVar16 + 1;
  bVar10 = *pbVar16;
  pcVar7 = (char *)(int)(char)bVar10;
  *(int *)(unaff_BP + 0xc) = (int)pbVar14;
  if ((int)pcVar7 < 0) goto switchD_1000_2105_caseD_0;
  if (0x15 < (byte)pcVar7[0x3d88]) goto switchD_1000_2105_default;
                    /* WARNING: Switch is manually overridden */
  switch(pcVar7[0x3d88]) {
  default:
    goto switchD_1000_2105_caseD_0;
  case '\x03':
    goto switchD_1000_2105_caseD_3;
  case '\x04':
    *(byte *)(unaff_BP + -1) = *(byte *)(unaff_BP + -1) | 1;
    pbVar16 = pbVar14;
    goto LAB_1000_20ea;
  case '\x05':
    LOCK();
    iVar5 = *(int *)(unaff_BP + -10);
    *(int *)(unaff_BP + -10) = (int)(pcVar7 + -0x30);
    UNLOCK();
    pbVar16 = pbVar14;
    if (-1 < iVar5) {
      *(int *)(unaff_BP + -10) = *(int *)(unaff_BP + -10) + iVar5 * 10;
    }
    goto LAB_1000_20ea;
  case '\x06':
    goto switchD_1000_2105_caseD_6;
  case '\a':
  case '\b':
    uVar12 = 10;
    break;
  case '\t':
    uVar12 = 0;
    break;
  case '\n':
    __scantod(*(undefined2 *)(unaff_BP + 4),*(undefined2 *)(unaff_BP + 6),
              *(undefined2 *)(unaff_BP + 8),*(undefined2 *)(unaff_BP + 10),
              *(uint *)(unaff_BP + -10) & 0x7fff,unaff_BP + -6);
    cVar18 = '\0';
    cVar4 = *(int *)(unaff_BP + -8) < 0;
    if (*(int *)(unaff_BP + -8) < 1) {
      __scanpop();
      if (cVar18 != cVar4) goto switchD_1000_2105_default;
      goto switchD_1000_2105_caseD_0;
    }
    if ((*(byte *)(unaff_BP + -1) & 1) == 0) {
      __scan_next_arg();
      *(int *)(unaff_BP + -4) = *(int *)(unaff_BP + -4) + 1;
      if ((*(byte *)(unaff_BP + -1) & 4) == 0) {
        if ((*(byte *)(unaff_BP + -1) & 8) == 0) {
          uVar12 = 0;
        }
        else {
          uVar12 = 8;
        }
      }
      else {
        uVar12 = 4;
      }
      __scanrslt(pcVar7,uVar9,uVar12);
    }
    else {
      __scanpop();
    }
    goto LAB_1000_2061;
  case '\v':
    *(byte *)(unaff_BP + -1) = *(byte *)(unaff_BP + -1) | 8;
    pbVar16 = pbVar14;
    goto LAB_1000_20ea;
  case '\f':
    *(byte *)(unaff_BP + -1) = *(byte *)(unaff_BP + -1) | 2;
    pbVar16 = pbVar14;
    goto LAB_1000_20ea;
  case '\r':
    *(byte *)(unaff_BP + -1) = *(byte *)(unaff_BP + -1) | 4;
    pbVar16 = pbVar14;
    goto LAB_1000_20ea;
  case '\x0e':
    uVar12 = 8;
    break;
  case '\x0f':
    iVar5 = 0x22a7;
    goto LAB_1000_2464;
  case '\x10':
    goto switchD_1000_2105_caseD_10;
  case '\x11':
    pbVar16 = pbVar14;
    if ((*(byte *)(unaff_BP + -1) & 1) == 0) goto LAB_1000_21a9;
    goto LAB_1000_20ea;
  case '\x12':
    uVar12 = 0x10;
    break;
  case '\x13':
    iVar5 = 0x21c3;
    goto LAB_1000_2464;
  case '\x14':
    *(byte *)(unaff_BP + -1) = *(byte *)(unaff_BP + -1) & 0xdf;
    pbVar16 = pbVar14;
    goto LAB_1000_20ea;
  case '\x15':
    goto switchD_1000_2105_caseD_15;
  }
  if ((((uint)pcVar7 & 0x20) == 0) &&
     (pcVar7 != (char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x29)) {
    *(byte *)(unaff_BP + -1) = *(byte *)(unaff_BP + -1) | 4;
  }
  __scantol(*(undefined2 *)(unaff_BP + 4),*(undefined2 *)(unaff_BP + 6),
            *(undefined2 *)(unaff_BP + 8),*(undefined2 *)(unaff_BP + 10),uVar12,
            *(uint *)(unaff_BP + -10) & 0x7fff,unaff_BP + -6);
  if (0 < *(int *)(unaff_BP + -8)) {
    if ((*(byte *)(unaff_BP + -1) & 1) == 0) {
      *(int *)(unaff_BP + -4) = *(int *)(unaff_BP + -4) + 1;
LAB_1000_21a9:
      uVar19 = __scan_next_arg();
      *(int *)pcVar7 = (int)uVar19;
      if ((*(byte *)(unaff_BP + -1) & 4) != 0) {
        *(int *)(pcVar7 + 2) = (int)((ulong)uVar19 >> 0x10);
      }
    }
    goto LAB_1000_2061;
  }
  if (*(int *)(unaff_BP + -8) < 0) goto switchD_1000_2105_default;
  goto switchD_1000_2105_caseD_0;
switchD_1000_2105_caseD_15:
  *(byte *)(unaff_BP + -1) = *(byte *)(unaff_BP + -1) | 0x20;
  pbVar16 = pbVar14;
  goto LAB_1000_20ea;
switchD_1000_2105_caseD_10:
  puVar15 = (undefined2 *)(unaff_BP + -0x2a);
  for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
    puVar3 = puVar15;
    puVar15 = puVar15 + 1;
    *puVar3 = 0;
  }
  pbVar13 = pbVar16 + 2;
  bVar10 = *pbVar14;
  *(byte *)(unaff_BP + -1) = *(byte *)(unaff_BP + -1) & 0xef;
  if (bVar10 == 0x5e) {
    *(byte *)(unaff_BP + -1) = *(byte *)(unaff_BP + -1) | 0x10;
    pbVar1 = pbVar13;
    pbVar13 = pbVar16 + 3;
    bVar10 = *pbVar1;
  }
  uVar8 = (uint)bVar10;
LAB_1000_236b:
  pbVar14 = pbVar13;
  pbVar16 = (byte *)(uVar8 >> 3);
  bVar10 = '\x01' << ((byte)uVar8 & 7);
  iVar5 = (uint)bVar10 << 8;
  pbVar16[unaff_BP + -0x2a] = pbVar16[unaff_BP + -0x2a] | bVar10;
  uVar11 = uVar8;
  while( true ) {
    pbVar13 = pbVar14 + 1;
    bVar10 = *pbVar14;
    uVar8 = (uint)bVar10;
    if (bVar10 == 0) goto switchD_1000_2105_caseD_0;
    if (bVar10 == 0x5d) break;
    if (((bVar10 != 0x2d) || (bVar10 = (byte)uVar11, *pbVar13 < bVar10)) || (*pbVar13 == 0x5d))
    goto LAB_1000_236b;
    pbVar14 = pbVar14 + 2;
    cVar4 = *pbVar13 - bVar10;
    if (cVar4 != '\0') {
      uVar11 = (uint)(byte)(bVar10 + cVar4);
      do {
        bVar17 = iVar5 < 0;
        bVar10 = (char)((uint)iVar5 >> 8) << 1 | bVar17;
        iVar5 = (uint)bVar10 << 8;
        pbVar16 = pbVar16 + bVar17;
        pbVar16[unaff_BP + -0x2a] = pbVar16[unaff_BP + -0x2a] | bVar10;
        cVar4 = cVar4 + -1;
      } while (cVar4 != '\0');
    }
  }
  *(int *)(unaff_BP + 0xc) = (int)pbVar13;
  *(uint *)(unaff_BP + -10) = *(uint *)(unaff_BP + -10) & 0x7fff;
  iVar5 = *(int *)(unaff_BP + -10);
  if ((*(byte *)(unaff_BP + -1) & 1) == 0) {
    __scan_next_arg();
  }
  while (iVar6 = iVar5 + -1, 0 < iVar5) {
    *(int *)(unaff_BP + -6) = *(int *)(unaff_BP + -6) + 1;
    uVar8 = (*(code *)*(undefined2 *)(unaff_BP + 4))
                      (*(undefined2 *)(unaff_BP + 8),*(undefined2 *)(unaff_BP + 10));
    if ((int)uVar8 < 0) {
      if ((iVar5 < *(int *)(unaff_BP + -10)) && ((*(byte *)(unaff_BP + -1) & 1) == 0)) {
        *pbVar16 = 0;
        *(int *)(unaff_BP + -4) = *(int *)(unaff_BP + -4) + 1;
      }
      goto switchD_1000_2105_default;
    }
    if ((*(byte *)(unaff_BP + (uVar8 >> 3) + -0x2a) & '\x01' << ((byte)uVar8 & 7)) == 0) {
      if ((*(byte *)(unaff_BP + -1) & 0x10) == 0) goto LAB_1000_2406;
    }
    else if ((*(byte *)(unaff_BP + -1) & 0x10) != 0) {
LAB_1000_2406:
      (*(code *)*(undefined2 *)(unaff_BP + 6))
                (uVar8,*(undefined2 *)(unaff_BP + 8),*(undefined2 *)(unaff_BP + 10));
      *(int *)(unaff_BP + -6) = *(int *)(unaff_BP + -6) + -1;
      if (iVar5 < *(int *)(unaff_BP + -10)) break;
      goto LAB_1000_2427;
    }
    iVar5 = iVar6;
    if ((*(byte *)(unaff_BP + -1) & 1) == 0) {
      pbVar1 = pbVar16;
      pbVar16 = pbVar16 + 1;
      *pbVar1 = (byte)uVar8;
    }
  }
  if ((*(byte *)(unaff_BP + -1) & 1) == 0) {
    *(int *)(unaff_BP + -4) = *(int *)(unaff_BP + -4) + 1;
LAB_1000_2427:
    if ((*(byte *)(unaff_BP + -1) & 1) == 0) {
      *pbVar16 = 0;
    }
  }
  goto LAB_1000_2061;
LAB_1000_2464:
  *(int *)(unaff_BP + -6) = *(int *)(unaff_BP + -6) + 1;
  iVar6 = (*(code *)*(undefined2 *)(unaff_BP + 4))
                    (*(undefined2 *)(unaff_BP + 8),*(undefined2 *)(unaff_BP + 10));
  if (iVar6 < 1) {
    if (iVar6 != 0) {
switchD_1000_2105_default:
      (*(code *)*(undefined2 *)(unaff_BP + 6))
                (0xffff,*(undefined2 *)(unaff_BP + 8),*(undefined2 *)(unaff_BP + 10));
      *(int *)(unaff_BP + -4) = *(int *)(unaff_BP + -4) - (uint)(*(int *)(unaff_BP + -4) == 0);
switchD_1000_2105_caseD_0:
      return *(undefined2 *)(unaff_BP + -4);
    }
LAB_1000_2483:
                    /* WARNING: Could not recover jumptable at 0x00012487. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar9 = (*(code *)(iVar5 + 3))();
    return uVar9;
  }
  if (((char)iVar6 < '\0') || (*(char *)(iVar6 + 0x3d88) != '\x01')) goto LAB_1000_2483;
  goto LAB_1000_2464;
switchD_1000_2105_caseD_6:
  if ((*(byte *)(unaff_BP + -1) & 1) == 0) {
    __scan_next_arg();
  }
  iVar5 = *(int *)(unaff_BP + -10);
  bVar17 = iVar5 == 0;
  if (iVar5 < 0) {
    iVar5 = 1;
  }
  if (bVar17) goto LAB_1000_233b;
  goto LAB_1000_231d;
  while( true ) {
    if ((*(byte *)(unaff_BP + -1) & 1) == 0) {
      pcVar2 = pcVar7;
      pcVar7 = pcVar7 + 1;
      *pcVar2 = (char)iVar6;
    }
    iVar6 = iVar5 + -1;
    bVar17 = iVar5 < 1;
    iVar5 = iVar6;
    if (iVar6 == 0 || bVar17) break;
LAB_1000_231d:
    *(int *)(unaff_BP + -6) = *(int *)(unaff_BP + -6) + 1;
    iVar6 = (*(code *)*(undefined2 *)(unaff_BP + 4))
                      (*(undefined2 *)(unaff_BP + 8),*(undefined2 *)(unaff_BP + 10));
    if (iVar6 < 0) goto switchD_1000_2105_default;
  }
LAB_1000_233b:
  if ((*(byte *)(unaff_BP + -1) & 1) == 0) {
    *(int *)(unaff_BP + -4) = *(int *)(unaff_BP + -4) + 1;
  }
  goto LAB_1000_2061;
switchD_1000_2105_caseD_3:
LAB_1000_2071:
  iVar5 = (int)(char)bVar10;
  *(int *)(unaff_BP + -6) = *(int *)(unaff_BP + -6) + 1;
  iVar6 = (*(code *)*(undefined2 *)(unaff_BP + 4))
                    (*(undefined2 *)(unaff_BP + 8),*(undefined2 *)(unaff_BP + 10));
  if (iVar6 < 0) goto switchD_1000_2105_default;
  iVar6 = (int)(char)iVar6;
  if ((iVar5 < 0) || (*(char *)(iVar5 + 0x3d88) != '\x01')) {
    if (iVar6 != iVar5) {
      (*(code *)*(undefined2 *)(unaff_BP + 6))
                (iVar6,*(undefined2 *)(unaff_BP + 8),*(undefined2 *)(unaff_BP + 10));
      *(int *)(unaff_BP + -6) = *(int *)(unaff_BP + -6) + -1;
      goto switchD_1000_2105_caseD_0;
    }
  }
  else {
    while ((-1 < (char)iVar6 && (*(char *)(iVar6 + 0x3d88) == '\x01'))) {
      *(int *)(unaff_BP + -6) = *(int *)(unaff_BP + -6) + 1;
      iVar6 = (*(code *)*(undefined2 *)(unaff_BP + 4))
                        (*(undefined2 *)(unaff_BP + 8),*(undefined2 *)(unaff_BP + 10));
      if (iVar6 < 1) goto switchD_1000_2105_default;
    }
    (*(code *)*(undefined2 *)(unaff_BP + 6))
              (iVar6,*(undefined2 *)(unaff_BP + 8),*(undefined2 *)(unaff_BP + 10));
    *(int *)(unaff_BP + -6) = *(int *)(unaff_BP + -6) + -1;
  }
  goto LAB_1000_2064;
}


