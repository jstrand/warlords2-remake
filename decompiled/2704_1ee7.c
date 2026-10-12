
void __cdecl16near FUN_2704_1ee7(void)

{
  long lVar1;
  ulong uVar2;
  long lVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  char cVar10;
  uint uVar11;
  int unaff_SI;
  uint *unaff_DI;
  uint uVar12;
  undefined2 unaff_ES;
  bool bVar13;
  undefined4 uVar14;
  
  *(bool *)(unaff_DI + 5) = *(char *)(unaff_SI + 9) < '\0';
  iVar5 = (uint)*(byte *)(unaff_SI + 8) << 4;
  uVar9 = (uint)(byte)(((byte)iVar5 >> 4) + (char)((uint)iVar5 >> 8) * '\n') * 10000;
  uVar14 = FUN_2704_1eb8();
  cVar10 = (char)((ulong)uVar14 >> 0x18);
  lVar1 = (ulong)((uint)uVar14 + uVar9) * 10000;
  uVar6 = (uint)lVar1;
  uVar8 = (int)((ulong)lVar1 >> 0x10) +
          CONCAT11(cVar10,(char)((ulong)uVar14 >> 0x10) + cVar10 + CARRY2((uint)uVar14,uVar9)) *
          10000;
  uVar14 = FUN_2704_1eb8();
  uVar9 = (uint)CARRY2((uint)uVar14,uVar6);
  cVar10 = (char)((ulong)uVar14 >> 0x18);
  lVar1 = (ulong)((uint)uVar14 + uVar6) * 10000;
  uVar6 = (uint)lVar1;
  lVar3 = (ulong)(uVar8 + uVar9) * 10000 +
          CONCAT22(CONCAT11(cVar10,(char)((ulong)uVar14 >> 0x10) + cVar10 + CARRY2(uVar8,uVar9)) *
                   10000,(int)((ulong)lVar1 >> 0x10));
  uVar11 = (uint)lVar3;
  uVar14 = FUN_2704_1eb8();
  uVar8 = (uint)((ulong)uVar14 >> 0x10);
  uVar9 = (uint)CARRY2((uint)uVar14,uVar6);
  lVar1 = (ulong)((uint)uVar14 + uVar6) * 10000;
  uVar7 = (uint)lVar1;
  lVar1 = (ulong)(uVar11 + uVar8 + uVar9) * 10000 +
          CONCAT22(((int)((ulong)lVar3 >> 0x10) + uVar8 +
                   (uint)(CARRY2(uVar11,uVar8) || CARRY2(uVar11 + uVar8,uVar9))) * 10000,
                   (int)((ulong)lVar1 >> 0x10));
  uVar11 = (uint)lVar1;
  uVar12 = (uint)((ulong)lVar1 >> 0x10);
  uVar14 = FUN_2704_1eb8();
  uVar9 = (uint)CARRY2(uVar7,(uint)uVar14);
  uVar6 = (uint)CARRY2(uVar11,uVar9);
  uVar8 = 0x40;
  uVar11 = uVar11 + uVar9;
  uVar4 = (int)((ulong)uVar14 >> 0x10) + (uint)CARRY2(uVar12,uVar6);
  uVar7 = uVar7 + (uint)uVar14;
  uVar9 = uVar12 + uVar6;
  do {
    uVar12 = uVar9;
    uVar6 = uVar7;
    uVar7 = uVar4;
    uVar9 = uVar11;
    if (uVar7 != 0) goto joined_r0x00028fc9;
    uVar8 = uVar8 - 0x10;
    uVar11 = uVar6;
    uVar4 = uVar12;
  } while (uVar8 != 0);
  uVar8 = 0xc001;
LAB_2704_1f96:
  *unaff_DI = uVar6;
  unaff_DI[1] = uVar9;
  unaff_DI[2] = uVar12;
  unaff_DI[3] = uVar7;
  unaff_DI[4] = uVar8;
  return;
joined_r0x00028fc9:
  for (; -1 < (int)uVar7; uVar7 = uVar7 * 2 + (uint)((uVar2 & 0x10000) != 0)) {
    uVar8 = uVar8 - 1;
    bVar13 = (int)uVar6 < 0;
    uVar6 = uVar6 << 1;
    uVar2 = (ulong)CONCAT12(bVar13,uVar9) << 1;
    uVar9 = (uint)uVar2 | (uint)bVar13;
    bVar13 = (uVar2 & 0x10000) != 0;
    uVar2 = (ulong)CONCAT12(bVar13,uVar12) << 1;
    uVar12 = (uint)uVar2 | (uint)bVar13;
  }
  goto LAB_2704_1f96;
}


