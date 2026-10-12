
undefined4 __cdecl16near FUN_2704_0353(void)

{
  long lVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint *unaff_BP;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  undefined2 unaff_SS;
  bool bVar14;
  
  uVar10 = 0;
  uVar3 = 0;
  uVar7 = 0;
  if ((*(byte *)((int)unaff_BP + 0xf) & 0x80) != 0) {
    uVar3 = *unaff_BP;
    uVar7 = unaff_BP[1];
  }
  if ((*(byte *)((int)unaff_BP + 3) & 0x80) != 0) {
    bVar14 = CARRY2(uVar3,unaff_BP[6]);
    uVar3 = uVar3 + unaff_BP[6];
    uVar10 = (uint)bVar14;
    bVar14 = CARRY2(uVar7,unaff_BP[7]);
    uVar5 = uVar7 + unaff_BP[7];
    uVar7 = uVar5 + uVar10;
    uVar10 = (uint)(bVar14 || CARRY2(uVar5,uVar10));
    if ((*(byte *)((int)unaff_BP + 0xf) & 0x80) != 0) {
      uVar10 = uVar10 - 1;
    }
  }
  uVar6 = 0;
  uVar4 = (uint)((ulong)unaff_BP[6] * (ulong)unaff_BP[2] >> 0x10);
  uVar2 = (uint)((ulong)unaff_BP[6] * (ulong)unaff_BP[2]);
  uVar5 = (uint)CARRY2(uVar3,uVar2);
  uVar11 = uVar7 + uVar4;
  uVar8 = uVar11 + uVar5;
  uVar10 = uVar10 + (CARRY2(uVar7,uVar4) || CARRY2(uVar11,uVar5));
  if (unaff_BP[8] != 0) {
    lVar1 = (ulong)unaff_BP[8] * (ulong)*unaff_BP;
    uVar5 = (uint)((ulong)lVar1 >> 0x10);
    uVar3 = (uint)CARRY2(uVar3 + uVar2,(uint)lVar1);
    uVar7 = uVar8 + uVar5;
    uVar6 = uVar7 + uVar3;
    uVar11 = uVar10 + (CARRY2(uVar8,uVar5) || CARRY2(uVar7,uVar3));
    uVar5 = (uint)((ulong)unaff_BP[8] * (ulong)unaff_BP[1] >> 0x10);
    uVar3 = (uint)((ulong)unaff_BP[8] * (ulong)unaff_BP[1]);
    uVar8 = uVar6 + uVar3;
    uVar3 = (uint)CARRY2(uVar6,uVar3);
    uVar7 = uVar11 + uVar5;
    uVar10 = uVar7 + uVar3;
    uVar6 = (uint)(CARRY2(uVar11,uVar5) || CARRY2(uVar7,uVar3));
  }
  uVar4 = (uint)((ulong)unaff_BP[6] * (ulong)unaff_BP[3] >> 0x10);
  uVar3 = (uint)((ulong)unaff_BP[6] * (ulong)unaff_BP[3]);
  uVar9 = uVar8 + uVar3;
  uVar3 = (uint)CARRY2(uVar8,uVar3);
  uVar5 = uVar10 + uVar4;
  uVar12 = uVar5 + uVar3;
  uVar8 = (uint)((ulong)unaff_BP[7] * (ulong)unaff_BP[2] >> 0x10);
  uVar2 = (uint)((ulong)unaff_BP[7] * (ulong)unaff_BP[2]);
  uVar7 = (uint)CARRY2(uVar9,uVar2);
  uVar11 = uVar12 + uVar8;
  uVar13 = uVar11 + uVar7;
  uVar3 = uVar6 + (CARRY2(uVar10,uVar4) || CARRY2(uVar5,uVar3)) +
          (uint)(CARRY2(uVar12,uVar8) || CARRY2(uVar11,uVar7));
  if (unaff_BP[9] != 0) {
    lVar1 = (ulong)unaff_BP[9] * (ulong)*unaff_BP;
    uVar10 = (uint)((ulong)lVar1 >> 0x10);
    uVar7 = (uint)CARRY2(uVar9 + uVar2,(uint)lVar1);
    bVar14 = CARRY2(uVar13,uVar10);
    uVar10 = uVar13 + uVar10;
    uVar13 = uVar10 + uVar7;
    uVar3 = uVar3 + (bVar14 || CARRY2(uVar10,uVar7));
  }
  uVar10 = 0;
  uVar6 = (uint)((ulong)unaff_BP[7] * (ulong)unaff_BP[3] >> 0x10);
  uVar7 = (uint)((ulong)unaff_BP[7] * (ulong)unaff_BP[3]);
  uVar2 = uVar13 + uVar7;
  uVar7 = (uint)CARRY2(uVar13,uVar7);
  uVar5 = uVar3 + uVar6;
  uVar11 = uVar5 + uVar7;
  uVar3 = (uint)(CARRY2(uVar3,uVar6) || CARRY2(uVar5,uVar7));
  if (unaff_BP[8] != 0) {
    lVar1 = (ulong)unaff_BP[8] * (ulong)unaff_BP[2];
    uVar5 = (uint)((ulong)lVar1 >> 0x10);
    uVar7 = (uint)lVar1;
    bVar14 = CARRY2(uVar2,uVar7);
    uVar2 = uVar2 + uVar7;
    uVar7 = (uint)bVar14;
    uVar10 = uVar11 + uVar5;
    uVar4 = uVar10 + uVar7;
    uVar5 = uVar3 + (CARRY2(uVar11,uVar5) || CARRY2(uVar10,uVar7));
    uVar6 = (uint)((ulong)unaff_BP[8] * (ulong)unaff_BP[3] >> 0x10);
    uVar3 = (uint)((ulong)unaff_BP[8] * (ulong)unaff_BP[3]);
    uVar11 = uVar4 + uVar3;
    uVar7 = (uint)CARRY2(uVar4,uVar3);
    uVar10 = uVar5 + uVar6;
    uVar3 = uVar10 + uVar7;
    uVar10 = (uint)(CARRY2(uVar5,uVar6) || CARRY2(uVar10,uVar7));
  }
  if (unaff_BP[9] != 0) {
    lVar1 = (ulong)unaff_BP[9] * (ulong)unaff_BP[1];
    uVar5 = (uint)((ulong)lVar1 >> 0x10);
    uVar7 = (uint)lVar1;
    bVar14 = CARRY2(uVar2,uVar7);
    uVar2 = uVar2 + uVar7;
    uVar7 = (uint)bVar14;
    uVar6 = uVar11 + uVar5;
    uVar5 = (uint)(CARRY2(uVar11,uVar5) || CARRY2(uVar6,uVar7));
    uVar11 = uVar3 + uVar5;
    uVar4 = (uint)((ulong)unaff_BP[9] * (ulong)unaff_BP[2] >> 0x10);
    uVar7 = (uint)CARRY2(uVar6 + uVar7,(uint)((ulong)unaff_BP[9] * (ulong)unaff_BP[2]));
    uVar6 = uVar11 + uVar4;
    uVar10 = uVar10 + CARRY2(uVar3,uVar5) + (uint)(CARRY2(uVar11,uVar4) || CARRY2(uVar6,uVar7)) +
             (int)((ulong)unaff_BP[9] * (ulong)unaff_BP[3] >> 0x10) +
             (uint)CARRY2(uVar6 + uVar7,(uint)((ulong)unaff_BP[9] * (ulong)unaff_BP[3]));
  }
  return CONCAT22(uVar10,uVar2);
}


