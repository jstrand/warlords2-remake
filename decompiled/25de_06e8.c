
uint __cdecl16near FUN_25de_06e8(void)

{
  undefined2 *puVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  undefined2 *puVar6;
  undefined2 *puVar7;
  uint *puVar8;
  undefined2 unaff_ES;
  bool bVar9;
  
  uVar3 = DAT_4125_0120;
  uVar2 = *(uint *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0xc);
  *(uint *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0xc) = DAT_4125_0120;
  uVar4 = *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 4) + 1U >> 1;
  puVar6 = (undefined2 *)0x0;
  bVar9 = uVar2 <= uVar3;
  puVar7 = puVar6;
  if (bVar9) {
    puVar6 = (undefined2 *)((uVar4 - 1) * 2);
    puVar7 = puVar6;
  }
  for (; uVar4 != 0; uVar4 = uVar4 - 1) {
    puVar1 = puVar6;
    puVar6 = puVar6 + (uint)bVar9 * -2 + 1;
    *puVar7 = *puVar1;
    puVar7 = puVar7 + (uint)bVar9 * -2 + 1;
  }
  *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 10) = unaff_ES;
  if (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x1c] != -0x33) {
    uVar3 = FUN_25de_0763();
    iVar5 = *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 8);
    puVar8 = (uint *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0x1f);
    do {
      *puVar8 = uVar3;
      puVar8 = (uint *)((int)puVar8 + 5);
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
  }
  return uVar3;
}


