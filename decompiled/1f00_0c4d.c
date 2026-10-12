
void FUN_1f00_0c4d(undefined2 param_1,undefined4 param_2,int param_3,int param_4,int param_5,
                  undefined2 param_6,undefined4 param_7,int param_8,undefined2 param_9,
                  undefined4 param_10,int param_11,int param_12)

{
  undefined2 *puVar1;
  code *pcVar2;
  char cVar3;
  int iVar4;
  byte bVar5;
  char *pcVar6;
  uint uVar7;
  undefined2 extraout_DX;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int unaff_SI;
  undefined2 *puVar12;
  int iVar13;
  int iVar14;
  undefined2 uVar15;
  int iVar16;
  undefined2 uVar17;
  undefined2 uVar18;
  bool bVar19;
  
  uVar15 = (undefined2)((ulong)param_2 >> 0x10);
  iVar8 = (int)param_2;
  iVar14 = *(int *)(iVar8 + 8);
  bVar5 = DAT_451b_2a3e & 0x1f;
  iVar16 = (int)((ulong)param_7 >> 0x10);
  iVar9 = (int)param_7;
  uVar17 = (undefined2)((ulong)param_10 >> 0x10);
  iVar10 = (int)param_10;
  uVar7 = (uint)DAT_451b_2a3e;
  param_11 = param_11 >> (DAT_451b_2a3e & 0x1f);
  iVar4 = param_12 * *(int *)(iVar10 + 8) * 2 + param_11;
  uVar18 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
  if ((*(uint *)(iVar8 + 4) < *(uint *)((int)DAT_4125_34e7 + 4)) ||
     (*(uint *)(iVar8 + 6) < *(uint *)((int)DAT_4125_34e7 + 6))) {
    bVar19 = false;
  }
  else {
    bVar19 = true;
  }
  if (((!bVar19) && (*(long *)(iVar8 + 10) != 0)) && ((*(uint *)(iVar8 + 0x35) & 10) == 0)) {
    for (iVar13 = 0; iVar13 < (int)(uint)*(byte *)(iVar8 + 0x34); iVar13 = iVar13 + 1) {
      iVar11 = iVar8 + iVar13 * 4;
      uVar18 = mem_lock(*(undefined2 *)(iVar11 + 10),*(undefined2 *)(iVar11 + 0xc));
      iVar11 = iVar8 + iVar13 * 4;
      *(int *)(iVar11 + 0x1c) = param_11;
      *(undefined2 *)(iVar11 + 0x1a) = uVar18;
    }
  }
  if ((iVar16 == 0x4125) && (iVar9 == 0x342a)) {
    if ((*(long *)(iVar10 + 10) != 0) && ((*(uint *)(iVar10 + 0x35) & 10) == 0)) {
      for (iVar9 = 0; iVar9 < (int)(uint)*(byte *)(iVar10 + 0x34); iVar9 = iVar9 + 1) {
        iVar16 = iVar10 + iVar9 * 4;
        uVar18 = mem_lock(*(undefined2 *)(iVar16 + 10),*(undefined2 *)(iVar16 + 0xc));
        iVar16 = iVar10 + iVar9 * 4;
        *(int *)(iVar16 + 0x1c) = param_11;
        *(undefined2 *)(iVar16 + 0x1a) = uVar18;
      }
    }
    for (iVar9 = 0; iVar9 < (int)(uint)*(byte *)(iVar8 + 0x34); iVar9 = iVar9 + 1) {
      if (((bVar19) &&
          (iVar16 = iVar8 + iVar9 * 4, *(int *)(iVar16 + 10) != 0 || *(int *)(iVar16 + 0xc) != 0))
         && ((*(uint *)(iVar8 + 0x35) & 10) == 0)) {
        iVar16 = iVar8 + iVar9 * 4;
        uVar18 = mem_lock(*(undefined2 *)(iVar16 + 10),*(undefined2 *)(iVar16 + 0xc));
        iVar16 = iVar8 + iVar9 * 4;
        *(int *)(iVar16 + 0x1c) = param_11;
        *(undefined2 *)(iVar16 + 0x1a) = uVar18;
      }
      FUN_1e68_0018(iVar9);
      FUN_1e68_0059((int)((ulong)*(undefined4 *)(iVar8 + iVar9 * 4 + 0x1a) >> 0x10),
                    param_4 * iVar14 * 2 + (param_3 >> bVar5),*(undefined2 *)(iVar8 + 8),
                    (int)((ulong)((undefined4 *)&DAT_4125_3444)[iVar9] >> 0x10),
                    param_8 >> (DAT_451b_2a3e & 0x1f),param_9,param_5 >> (DAT_451b_2a40 & 0x1f),
                    param_6,(int)((ulong)*(undefined4 *)(iVar10 + 0x1a) >> 0x10),iVar4,
                    *(undefined2 *)(iVar10 + 8));
      iVar16 = iVar8 + iVar9 * 4;
      if ((*(int *)(iVar16 + 10) != 0 || *(int *)(iVar16 + 0xc) != 0) &&
         ((*(uint *)(iVar8 + 0x35) & 10) == 0)) {
        iVar16 = iVar8 + iVar9 * 4;
        mem_unlock(*(undefined2 *)(iVar16 + 10),*(undefined2 *)(iVar16 + 0xc));
      }
    }
    if ((*(long *)(iVar10 + 10) != 0) && ((*(uint *)(iVar10 + 0x35) & 10) == 0)) {
      for (iVar14 = 0; iVar14 < (int)(uint)*(byte *)(iVar10 + 0x34); iVar14 = iVar14 + 1) {
        iVar4 = iVar10 + iVar14 * 4;
        mem_unlock(*(undefined2 *)(iVar4 + 10),*(undefined2 *)(iVar4 + 0xc));
      }
    }
  }
  else {
    if ((*(long *)(iVar10 + 10) != 0) && ((*(uint *)(iVar10 + 0x35) & 10) == 0)) {
      for (iVar14 = 0; iVar14 < (int)(uint)*(byte *)(iVar10 + 0x34); iVar14 = iVar14 + 1) {
        iVar4 = iVar10 + iVar14 * 4;
        uVar15 = mem_lock(*(undefined2 *)(iVar4 + 10),*(undefined2 *)(iVar4 + 0xc));
        iVar4 = iVar10 + iVar14 * 4;
        *(int *)(iVar4 + 0x1c) = param_11;
        *(undefined2 *)(iVar4 + 0x1a) = uVar15;
      }
    }
    bVar5 = (byte)uVar7;
    if ((*(long *)(iVar9 + 10) != 0) && ((*(uint *)(iVar9 + 0x35) & 10) == 0)) {
      for (iVar14 = 0; bVar5 = (byte)uVar7, iVar14 < (int)(uint)*(byte *)(iVar9 + 0x34);
          iVar14 = iVar14 + 1) {
        iVar4 = iVar9 + iVar14 * 4;
        uVar17 = mem_lock(*(undefined2 *)(iVar4 + 10),*(undefined2 *)(iVar4 + 0xc));
        iVar4 = iVar9 + iVar14 * 4;
        *(int *)(iVar4 + 0x1c) = param_11;
        *(undefined2 *)(iVar4 + 0x1a) = uVar17;
      }
    }
    uVar15 = 0;
    pcVar2 = (code *)swi(0x21);
    (*pcVar2)();
    uVar17 = uRam00010291;
    DAT_4125_26ca = unaff_SI << (bVar5 & 0x1f);
    puVar12 = (undefined2 *)&DAT_451b_0000;
    for (pcVar6 = (char *)s_Your_Upkeep_4125_3201 + 5; pcVar6 != (char *)0x0; pcVar6 = pcVar6 + -1)
    {
      puVar1 = puVar12;
      puVar12 = (undefined2 *)((int)puVar12 + 1);
      *(undefined1 *)puVar1 = 0;
    }
    if (((0x14 < DAT_4125_3cfc) && (bVar19 = DAT_4125_007d < 3, !bVar19)) &&
       ((3 < DAT_4125_007d || (bVar19 = DAT_4125_007e < 0x1e, !bVar19)))) {
      pcVar2 = (code *)swi(0x21);
      (*pcVar2)();
      if (bVar19) {
LAB_1000_0109:
        abort();
        return;
      }
      pcVar2 = (code *)swi(0x21);
      (*pcVar2)(uVar15);
      if (bVar19) goto LAB_1000_0109;
      pcVar2 = (code *)swi(0x21);
      iVar14 = (*pcVar2)();
      if (bVar19) goto LAB_1000_0109;
      DAT_4125_0091 = iVar14 + 1;
      pcVar2 = (code *)swi(0x21);
      (*pcVar2)();
      if (bVar19) goto LAB_1000_0109;
      pcVar2 = (code *)swi(0x21);
      (*pcVar2)();
      if (bVar19) goto LAB_1000_0109;
    }
    pcVar2 = (code *)swi(0x1a);
    cVar3 = (*pcVar2)();
    DAT_4125_0081 = extraout_DX;
    DAT_4125_0083 = pcVar6;
    if (cVar3 == '\0') goto LAB_1000_0128;
    uVar17 = 0x40;
  }
  *(undefined1 *)((int)(undefined2 *)&DAT_4125_006f + 1) = 1;
LAB_1000_0128:
  __run_startup_procs();
  iVar14 = main(DAT_4125_006b,DAT_4125_006d,DAT_4125_006f,DAT_4125_0071,DAT_4125_0073);
  exit(iVar14);
  __run_exit_procs(0x3f54,0x3f2a);
  return;
}


