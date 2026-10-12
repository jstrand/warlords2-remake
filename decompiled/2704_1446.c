
void __cdecl16near FUN_2704_1446(void)

{
  long lVar1;
  undefined2 uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  uint uVar17;
  int iVar18;
  uint uVar19;
  uint uVar20;
  undefined2 unaff_SS;
  
  uVar2 = s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_;
  uVar3 = *(uint *)s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_;
  uVar14 = *(uint *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 2);
  uVar15 = *(uint *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 4);
  uVar16 = *(uint *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 6);
  uVar20 = 0;
  iVar18 = *(int *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 8);
  uVar4 = uVar3;
  uVar5 = uVar15;
  uVar11 = uVar16;
  if (iVar18 != -0x3fff) {
    if (iVar18 < -0xf) {
      uVar11 = 0;
      uVar4 = uVar14;
      uVar5 = uVar16;
      uVar14 = uVar15;
      iVar18 = iVar18 + 0x10;
      uVar20 = uVar3;
    }
    while (iVar18 + 1 == 0 || iVar18 < -1) {
      uVar3 = uVar11 & 1;
      uVar11 = uVar11 >> 1;
      uVar15 = uVar5 & 1;
      uVar20 = (uint)(CONCAT12((uVar4 & 1) != 0,uVar20) >> 1);
      uVar4 = (uint)(CONCAT12((uVar14 & 1) != 0,uVar4) >> 1);
      uVar5 = (uint)(CONCAT12(uVar3 != 0,uVar5) >> 1);
      uVar14 = (uint)(CONCAT12(uVar15 != 0,uVar14) >> 1);
      iVar18 = iVar18 + 1;
    }
  }
  uVar3 = (uint)CARRY2(uVar4,(uint)((int)uVar20 < 0));
  uVar15 = (uint)CARRY2(uVar14,uVar3);
  *(uint *)s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ = uVar4 + ((int)uVar20 < 0);
  *(int *)(uVar2 + 2) = uVar14 + uVar3;
  *(int *)(uVar2 + 4) = uVar5 + uVar15;
  *(int *)(uVar2 + 6) = uVar11 + CARRY2(uVar5,uVar15);
  uVar14 = 0;
  uVar3 = 0;
  if ((*(byte *)(uVar2 + 3) & 0x80) != 0) {
    uVar14 = *(int *)uVar2 << 1;
    uVar3 = *(int *)(uVar2 + 2) << 1 | (uint)(*(int *)uVar2 < 0);
  }
  lVar1 = (ulong)*(uint *)uVar2 * (ulong)*(uint *)(uVar2 + 4);
  uVar11 = (uint)((ulong)lVar1 >> 0x10);
  uVar20 = (uint)lVar1;
  uVar4 = uVar3 + uVar11 + (uint)CARRY2(uVar14,uVar20);
  uVar15 = (uint)CARRY2(uVar14 + uVar20,uVar20);
  uVar16 = uVar4 + uVar11;
  uVar5 = uVar16 + uVar15;
  uVar19 = (uint)(CARRY2(uVar3,uVar11) || CARRY2(uVar3 + uVar11,(uint)CARRY2(uVar14,uVar20))) +
           (uint)(CARRY2(uVar4,uVar11) || CARRY2(uVar16,uVar15));
  lVar1 = (ulong)*(uint *)(uVar2 + 2) * (ulong)*(uint *)(uVar2 + 4);
  uVar12 = (uint)((ulong)lVar1 >> 0x10);
  uVar14 = (uint)lVar1;
  uVar15 = uVar5 + uVar14;
  uVar3 = (uint)CARRY2(uVar5,uVar14);
  uVar4 = uVar19 + uVar12;
  uVar6 = uVar4 + uVar3;
  uVar16 = uVar15 + uVar14;
  uVar14 = (uint)CARRY2(uVar15,uVar14);
  uVar5 = uVar6 + uVar12;
  uVar7 = uVar5 + uVar14;
  lVar1 = (ulong)*(uint *)uVar2 * (ulong)*(uint *)(uVar2 + 6);
  uVar13 = (uint)((ulong)lVar1 >> 0x10);
  uVar20 = (uint)lVar1;
  uVar17 = uVar16 + uVar20;
  uVar15 = (uint)CARRY2(uVar16,uVar20);
  uVar11 = uVar7 + uVar13;
  uVar8 = uVar11 + uVar15;
  uVar16 = (uint)CARRY2(uVar17,uVar20);
  uVar10 = uVar8 + uVar13;
  uVar9 = uVar10 + uVar16;
  uVar20 = (uint)((int)(uVar17 + uVar20) < 0);
  uVar17 = uVar9 + uVar20;
  uVar12 = (uint)(CARRY2(uVar19,uVar12) || CARRY2(uVar4,uVar3)) +
           (uint)(CARRY2(uVar6,uVar12) || CARRY2(uVar5,uVar14)) +
           (uint)(CARRY2(uVar7,uVar13) || CARRY2(uVar11,uVar15)) +
           (uint)(CARRY2(uVar8,uVar13) || CARRY2(uVar10,uVar16)) + (uint)CARRY2(uVar9,uVar20);
  lVar1 = (ulong)*(uint *)(uVar2 + 4) * (ulong)*(uint *)(uVar2 + 4);
  uVar7 = (uint)((ulong)lVar1 >> 0x10);
  uVar3 = (uint)lVar1;
  uVar14 = uVar17 + uVar3;
  uVar3 = (uint)CARRY2(uVar17,uVar3);
  uVar16 = uVar12 + uVar7;
  uVar11 = uVar16 + uVar3;
  lVar1 = (ulong)*(uint *)(uVar2 + 6) * (ulong)*(uint *)(uVar2 + 2);
  uVar8 = (uint)((ulong)lVar1 >> 0x10);
  uVar5 = (uint)lVar1;
  uVar9 = uVar14 + uVar5;
  uVar14 = (uint)CARRY2(uVar14,uVar5);
  uVar20 = uVar11 + uVar8;
  uVar10 = uVar20 + uVar14;
  uVar15 = (uint)CARRY2(uVar9,uVar5);
  uVar4 = uVar10 + uVar8;
  uVar6 = uVar4 + uVar15;
  uVar10 = (uint)(CARRY2(uVar12,uVar7) || CARRY2(uVar16,uVar3)) +
           (uint)(CARRY2(uVar11,uVar8) || CARRY2(uVar20,uVar14)) +
           (uint)(CARRY2(uVar10,uVar8) || CARRY2(uVar4,uVar15));
  lVar1 = (ulong)*(uint *)(uVar2 + 6) * (ulong)*(uint *)(uVar2 + 4);
  uVar11 = (uint)((ulong)lVar1 >> 0x10);
  uVar20 = (uint)lVar1;
  uVar4 = uVar6 + uVar20;
  uVar3 = (uint)CARRY2(uVar6,uVar20);
  uVar15 = uVar10 + uVar11;
  uVar6 = uVar15 + uVar3;
  uVar14 = (uint)CARRY2(uVar4,uVar20);
  uVar16 = uVar6 + uVar11;
  lVar1 = (ulong)*(uint *)(uVar2 + 6) * (ulong)*(uint *)(uVar2 + 6) +
          CONCAT22((uint)(CARRY2(uVar10,uVar11) || CARRY2(uVar15,uVar3)) +
                   (uint)(CARRY2(uVar6,uVar11) || CARRY2(uVar16,uVar14)),uVar16 + uVar14);
  *(uint *)uVar2 = uVar9 + uVar5;
  *(int *)(uVar2 + 2) = uVar4 + uVar20;
  *(undefined2 *)(uVar2 + 4) = (int)lVar1;
  *(undefined2 *)(uVar2 + 6) = (int)((ulong)lVar1 >> 0x10);
  *(undefined1 *)(uVar2 + 10) = 0;
  *(undefined2 *)(uVar2 + 8) = 0;
  return;
}


