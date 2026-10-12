
int __cdecl16far FUN_25de_0ed1(void)

{
  char *pcVar1;
  char *pcVar2;
  byte bVar3;
  uint uVar4;
  code *pcVar5;
  long lVar6;
  undefined2 uVar7;
  char cVar8;
  undefined2 uVar9;
  byte extraout_AH;
  uint uVar10;
  uint uVar11;
  int iVar12;
  undefined2 in_BX;
  char *pcVar13;
  char *pcVar14;
  undefined2 unaff_SS;
  bool bVar15;
  undefined1 uVar16;
  undefined4 uVar17;
  char local_1f [8];
  uint local_17;
  uint local_f;
  
  uVar7 = DAT_1f00_1009;
  if ((((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0x13] & 1U) == 0) {
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0x13] = '\x01';
LAB_25de_0ef5:
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb)[0] = '\0';
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb)[1] = '\0';
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xd)[0] = '\x10';
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xd)[1] = '\0';
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xf)[0] = '\0';
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xf)[1] = '\0';
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x11)[0] = '\x10';
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x11)[1] = '\0';
    pcVar5 = (code *)swi(0x21);
    uVar9 = (*pcVar5)();
    if (((((byte)uVar9 < 10) && (DAT_f000_fffe < 0xfd)) && (DAT_f000_fffe != 0xfb)) &&
       (DAT_f000_fffe != 0xf9)) {
      if (0x30 < CONCAT11((byte)uVar9,(char)((uint)uVar9 >> 8))) {
        pcVar5 = (code *)swi(0x2f);
        cVar8 = (*pcVar5)();
        if (cVar8 == -0x80) {
          pcVar5 = (code *)swi(0x2f);
          (*pcVar5)();
          *(undefined2 *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14) = in_BX;
          ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x16)[0] = '\0';
          ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x16)[1] = -0x10;
          (**(code **)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14))();
          if (1 < extraout_AH) {
            uVar11 = (**(code **)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14))();
            if (uVar11 != 0) {
              uVar17 = (**(code **)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14))();
              if ((int)uVar17 != 0) {
                *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x18) =
                     (int)((ulong)uVar17 >> 0x10);
                uVar17 = (**(code **)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14))
                                   ();
                if ((int)uVar17 != 0) {
                  *(undefined2 *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb) =
                       in_BX;
                  *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xd) =
                       (int)((ulong)uVar17 >> 0x10);
                  uVar10 = (uint)((ulong)uVar11 * 0x400);
                  uVar4 = *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb);
                  iVar12 = *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xd);
                  *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xf) =
                       uVar10 + *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb
                                         );
                  *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x11) =
                       (int)((ulong)uVar11 * 0x400 >> 0x10) + iVar12 + (uint)CARRY2(uVar10,uVar4);
                }
                (**(code **)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14))();
                (**(code **)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14))();
                ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x18)[0] = '\0';
                ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x18)[1] = '\0';
              }
            }
            goto LAB_25de_1066;
          }
        }
      }
      pcVar5 = (code *)swi(0x15);
      uVar11 = (*pcVar5)();
      if (uVar11 != 0) {
        lVar6 = (ulong)uVar11 * 0x400;
        *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xf) = (int)lVar6;
        *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x11) =
             CONCAT11((char)((ulong)lVar6 >> 0x18),(char)((ulong)lVar6 >> 0x10) + '\x10');
        if (*(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14) != 0 ||
            *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x16) != 0) {
          ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0xd] = '\x11';
          ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb)[0] = '\0';
          ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb)[1] = '\0';
        }
        bVar15 = false;
        uVar16 = 1;
        FUN_25de_11ad();
        if (!bVar15) {
          pcVar14 = local_1f;
          pcVar13 = (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x1c;
          iVar12 = 5;
          do {
            if (iVar12 == 0) break;
            iVar12 = iVar12 + -1;
            pcVar2 = pcVar14;
            pcVar14 = pcVar14 + 1;
            pcVar1 = pcVar13;
            pcVar13 = pcVar13 + 1;
            uVar16 = *pcVar1 == *pcVar2;
          } while ((bool)uVar16);
          if ((bool)uVar16) {
            *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb) =
                 (int)((ulong)local_17 * (ulong)local_f);
            ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0xd] =
                 (char)((ulong)local_17 * (ulong)local_f >> 0x10) + '\x10';
          }
          bVar15 = true;
          pcVar13 = (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0xe;
          pcVar14 = (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x1c;
          iVar12 = 5;
          do {
            if (iVar12 == 0) break;
            iVar12 = iVar12 + -1;
            pcVar2 = pcVar13;
            pcVar13 = pcVar13 + 1;
            pcVar1 = pcVar14;
            pcVar14 = pcVar14 + 1;
            bVar15 = *pcVar1 == *pcVar2;
          } while (bVar15);
          if (bVar15) {
            bVar3 = ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x2a];
            if (((byte)((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0xd] <= bVar3) &&
               ((bVar3 != ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0xd] ||
                (*(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb) <
                 *(uint *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x28))))) {
              *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb) =
                   *(uint *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x28);
              ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0xd] = bVar3;
            }
          }
          goto LAB_25de_1066;
        }
      }
    }
    iVar12 = 0;
  }
  else {
    if ((((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0x13] & 2U) == 0)
    goto LAB_25de_0ef5;
LAB_25de_1066:
    iVar12 = *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xf) -
             *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb);
  }
  return iVar12;
}


