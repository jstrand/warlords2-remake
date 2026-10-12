
/* WARNING: Stack frame is not setup normally: Input value of stackpointer is not used */
/* WARNING: This function may have set the stack pointer */

void _start(void)

{
  char *pcVar1;
  undefined2 *puVar2;
  code *pcVar3;
  char cVar4;
  undefined2 uVar5;
  byte bVar6;
  uint uVar7;
  undefined2 uVar8;
  undefined2 extraout_DX;
  undefined2 extraout_DX_00;
  int iVar9;
  int iVar10;
  undefined1 *puVar11;
  int *piVar12;
  char *pcVar13;
  undefined1 *puVar14;
  undefined2 *puVar15;
  undefined2 unaff_ES;
  bool bVar16;
  undefined4 uVar17;
  
  puVar11 = (undefined1 *)((int)(undefined2 *)&DAT_4125_007f + 1);
  uRam00010291 = 0x4125;
  pcVar3 = (code *)swi(0x21);
  uVar17 = (*pcVar3)();
  uVar5 = s_Borland_C_____Copyright_1991_Bor_4125_0004._40_2_;
  iVar10 = DAT_4125_0002;
  uVar8 = (undefined2)((ulong)uVar17 >> 0x10);
  *(undefined2 *)&DAT_4125_007d = (int)uVar17;
  *(undefined2 *)&DAT_4125_007b = unaff_ES;
  *(undefined2 *)(char *)&DAT_4125_0077 = uVar5;
  *(int *)&DAT_4125_0091 = iVar10;
  *(undefined2 *)(puVar11 + -2) = 0x28;
  __save_vectors();
  iVar9 = 0;
  pcVar13 = (char *)0x0;
  uVar7 = 0x7fff;
LAB_1000_0037:
  do {
    if (uVar7 != 0) {
      uVar7 = uVar7 - 1;
      pcVar1 = pcVar13;
      pcVar13 = pcVar13 + 1;
      if (*pcVar1 != '\0') goto LAB_1000_0037;
    }
    if (uVar7 == 0) goto LAB_1000_007e;
    iVar9 = iVar9 + 1;
  } while (*pcVar13 != '\0');
  *(int *)&DAT_4125_0075 = -(uVar7 | 0x8000);
  *(uint *)&DAT_4125_0079 = iVar9 * 4 + 0x10U & 0xfff0;
  if (DAT_4125_26ca < 0x200) {
    DAT_4125_26ca = (undefined1 *)0x200;
  }
  bVar6 = 4;
  uVar7 = ((uint)DAT_4125_26ca >> 4) + 1;
  if (iVar10 + 0xb7c4U < uVar7) {
LAB_1000_007e:
    abort();
    return;
  }
  iVar10 = ((uint)DAT_4125_26ca >> 4) + 0x483d;
  *(int *)&DAT_4125_0089 = iVar10;
  *(int *)&DAT_4125_008d = iVar10;
  piVar12 = (int *)(puVar11 + -2);
  *(uint *)(puVar11 + -2) = uVar7;
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  uVar5 = uRam00010291;
  puVar14 = (undefined1 *)(*piVar12 << (bVar6 & 0x1f));
  puVar15 = (undefined2 *)&DAT_451b_0000;
  DAT_4125_26ca = puVar14;
  for (pcVar13 = (char *)s_Your_Upkeep_4125_3201 + 5; pcVar13 != (char *)0x0; pcVar13 = pcVar13 + -1
      ) {
    puVar2 = puVar15;
    puVar15 = (undefined2 *)((int)puVar15 + 1);
    *(undefined1 *)puVar2 = 0;
  }
  if (0x14 < *(uint *)&DAT_4125_3cfc) {
    bVar16 = *(byte *)&DAT_4125_007d < 3;
    if ((!bVar16) &&
       ((3 < *(byte *)&DAT_4125_007d || (bVar16 = *(byte *)&DAT_4125_007e < 0x1e, !bVar16)))) {
      pcVar3 = (code *)swi(0x21);
      (*pcVar3)();
      if (!bVar16) {
        pcVar3 = (code *)swi(0x21);
        (*pcVar3)();
        if (!bVar16) {
          pcVar3 = (code *)swi(0x21);
          iVar10 = (*pcVar3)();
          if (!bVar16) {
            *(int *)&DAT_4125_0091 = iVar10 + 1;
            pcVar3 = (code *)swi(0x21);
            (*pcVar3)();
            if (!bVar16) {
              pcVar3 = (code *)swi(0x21);
              (*pcVar3)();
              if (!bVar16) goto LAB_1000_010c;
            }
          }
        }
      }
      abort();
      return;
    }
  }
LAB_1000_010c:
  pcVar3 = (code *)swi(0x1a);
  cVar4 = (*pcVar3)();
  *(undefined2 *)&DAT_4125_0081 = extraout_DX_00;
  *(undefined2 *)&DAT_4125_0083 = pcVar13;
  if (cVar4 != '\0') {
    DAT_0000_0470 = 1;
  }
  *(undefined2 *)(puVar14 + -2) = 0x138;
  __run_startup_procs();
  *(undefined2 *)(puVar14 + -2) = *(undefined2 *)&DAT_4125_0073;
  *(undefined2 *)(puVar14 + -4) = *(undefined2 *)&DAT_4125_0071;
  *(undefined2 *)(puVar14 + -6) = *(undefined2 *)&DAT_4125_006f;
  *(undefined2 *)(puVar14 + -8) = *(undefined2 *)&DAT_4125_006d;
  *(undefined2 *)(puVar14 + -10) = *(undefined2 *)&DAT_4125_006b;
  *(undefined2 *)(puVar14 + -0xc) = 0x1000;
  *(undefined2 *)(puVar14 + -0xe) = 0x151;
  uVar5 = main();
  *(undefined2 *)(puVar14 + -0xc) = uVar5;
  *(undefined2 *)(puVar14 + -0xe) = 0x1a4c;
  *(undefined2 *)(puVar14 + -0x10) = 0x157;
  exit(*(int *)(puVar14 + -0xc));
  *(undefined2 *)(puVar14 + -0xe) = 0x3f2a;
  *(undefined2 *)(puVar14 + -0x10) = 0x3f54;
  *(undefined2 *)(puVar14 + -0x12) = 0x167;
  __run_exit_procs();
  return;
}


