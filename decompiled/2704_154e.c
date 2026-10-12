
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_2704_154e(int param_1,byte param_2)

{
  uint *puVar1;
  byte bVar2;
  int *piVar3;
  uint uVar4;
  undefined2 uVar5;
  byte bVar6;
  uint in_CX;
  uint uVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  uint in_BX;
  int iVar12;
  uint uVar13;
  int *piVar14;
  int iVar15;
  uint *puVar16;
  uint *puVar17;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  bool bVar18;
  undefined4 uVar19;
  
  FUN_2704_0e87();
  puVar16 = (uint *)((uint)param_2 * 0xc + -0xc + param_1);
  while( true ) {
    uVar5 = s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_;
    bVar2 = param_2 - 1;
    if (bVar2 == 0 || (char)param_2 < '\x01') break;
    *(byte *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 10) =
         *(byte *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 10) ^
         *(byte *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 0x16);
    uVar19 = FUN_2704_0353();
    *(int *)uVar5 = (int)uVar19;
    *(uint *)(uVar5 + 2) = in_BX;
    *(uint *)(uVar5 + 4) = in_CX;
    *(undefined2 *)(uVar5 + 6) = (int)((ulong)uVar19 >> 0x10);
    puVar17 = puVar16 + -6;
    in_BX = *puVar17;
    in_CX = puVar16[-5];
    uVar7 = puVar16[-4];
    uVar10 = puVar16[-3];
    bVar6 = (byte)puVar16[-1];
    puVar16 = puVar17;
    param_2 = bVar2;
    if (bVar6 == *(byte *)(uVar5 + 10)) {
      puVar1 = (uint *)uVar5;
      uVar4 = *puVar1;
      *puVar1 = *puVar1 + in_BX;
      puVar1 = (uint *)(uVar5 + 2);
      uVar9 = *puVar1;
      uVar13 = *puVar1;
      *puVar1 = uVar13 + in_CX + (uint)CARRY2(uVar4,in_BX);
      puVar1 = (uint *)(uVar5 + 4);
      uVar9 = (uint)(CARRY2(uVar9,in_CX) || CARRY2(uVar13 + in_CX,(uint)CARRY2(uVar4,in_BX)));
      uVar13 = *puVar1;
      uVar4 = *puVar1;
      *puVar1 = uVar4 + uVar7 + uVar9;
      *(int *)(uVar5 + 6) =
           *(int *)(uVar5 + 6) + uVar10 +
           (uint)(CARRY2(uVar13,uVar7) || CARRY2(uVar4 + uVar7,uVar9));
    }
    else {
      bVar18 = in_BX < *(uint *)uVar5;
      in_BX = in_BX - *(uint *)uVar5;
      uVar4 = (uint)bVar18;
      bVar18 = in_CX < *(uint *)(uVar5 + 2);
      uVar13 = in_CX - *(uint *)(uVar5 + 2);
      in_CX = uVar13 - uVar4;
      uVar4 = (uint)(bVar18 || uVar13 < uVar4);
      uVar13 = uVar7 - *(uint *)(uVar5 + 4);
      uVar9 = uVar13 - uVar4;
      uVar7 = (uint)(uVar7 < *(uint *)(uVar5 + 4) || uVar13 < uVar4);
      uVar4 = uVar10 - *(uint *)(uVar5 + 6);
      uVar13 = uVar4 - uVar7;
      if (uVar10 < *(uint *)(uVar5 + 6) || uVar4 < uVar7) {
        uVar10 = ~uVar9;
        uVar7 = ~in_CX;
        bVar18 = in_BX == 0;
        in_BX = -in_BX;
        in_CX = uVar7 + bVar18;
        uVar7 = (uint)CARRY2(uVar7,(uint)bVar18);
        uVar9 = uVar10 + uVar7;
        uVar13 = ~uVar13 + (uint)CARRY2(uVar10,uVar7);
        bVar6 = bVar6 ^ 1;
      }
      *(uint *)uVar5 = in_BX;
      *(uint *)(uVar5 + 2) = in_CX;
      *(uint *)(uVar5 + 4) = uVar9;
      *(uint *)(uVar5 + 6) = uVar13;
      *(byte *)(uVar5 + 10) = bVar6;
    }
  }
  uVar7 = *(uint *)s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_;
  uVar10 = *(uint *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 2);
  uVar4 = *(uint *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 4);
  uVar13 = *(uint *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 6);
  piVar14 = (int *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 0xc);
  bVar18 = *(char *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 10) != '\x01';
  if (bVar18) {
    uVar9 = (uint)(CONCAT12((uVar13 & 1) != 0,uVar4) >> 1);
    uVar4 = (uint)(CONCAT12((uVar4 & 1) != 0,uVar10) >> 1);
    uVar10 = (uint)(CONCAT12((uVar10 & 1) != 0,uVar7) >> 1);
    uVar7 = (uint)((uVar7 & 1) != 0);
    iVar12 = uVar10 + uVar7;
    uVar7 = (uint)CARRY2(uVar10,uVar7);
    iVar8 = uVar4 + uVar7;
    uVar7 = (uint)CARRY2(uVar4,uVar7);
    iVar11 = uVar9 + uVar7;
    iVar15 = (int)(((ulong)uVar13 | 0x10000) >> 1) + (uint)CARRY2(uVar9,uVar7);
  }
  else {
    iVar12 = -uVar7;
    iVar8 = ~uVar10 + (uint)(uVar7 == 0);
    uVar7 = (uint)CARRY2(~uVar10,(uint)(uVar7 == 0));
    iVar11 = ~uVar4 + uVar7;
    iVar15 = ~uVar13 + (uint)CARRY2(~uVar4,uVar7);
  }
  piVar3 = (int *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + 0xe);
  s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ = piVar14;
  *piVar14 = iVar12;
  *piVar3 = iVar8;
  *(int *)(uVar5 + 0x10) = iVar11;
  *(int *)(uVar5 + 0x12) = iVar15;
  *(uint *)(uVar5 + 0x14) = (uint)bVar18;
  *(undefined1 *)(uVar5 + 0x16) = 0;
  return;
}


