
uint __cdecl16near FUN_1d28_0a84(void)

{
  undefined2 uVar1;
  undefined2 *puVar2;
  undefined2 uVar3;
  uint in_AX;
  byte bVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  int *piVar8;
  undefined2 unaff_BP;
  uint uVar9;
  int iVar10;
  uint uStack_4;
  
  if (DAT_4125_3292 == 0) {
    return in_AX;
  }
  do {
    uVar9 = 0;
    iVar10 = 0;
    iVar6 = DAT_4125_327c;
    do {
      if (((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x80U) != 0) &&
         ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x20U) == 0)) {
        uVar7 = *(uint *)&DAT_4125_0002;
        iVar5 = iVar6 + uVar7;
        if ((iVar5 != DAT_4125_327e) &&
           ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x80U) == 0)) {
          uVar7 = uVar7 + *(int *)&DAT_4125_0002;
        }
        if ((*(int *)0x0 != 0) &&
           ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x80U) == 0)) {
          uVar7 = uVar7 + *(int *)&DAT_4125_0002;
        }
        if (in_AX <= uVar7) {
          uVar9 = 0;
          uStack_4 = in_AX;
          goto LAB_1d28_0b1e;
        }
        if (uVar9 < uVar7) {
          uVar9 = uVar7;
          iVar10 = iVar6;
        }
      }
      iVar6 = iVar6 + *(int *)&DAT_4125_0002;
      if (iVar6 == DAT_4125_327e) {
        iVar6 = DAT_4125_327a;
      }
    } while (iVar6 != DAT_4125_327c);
    if (uVar9 == 0) {
      return in_AX;
    }
    uStack_4 = in_AX - uVar9;
    iVar6 = iVar10;
LAB_1d28_0b1e:
    iVar10 = (int)*(uint *)&DAT_4125_0002 >> 5;
    if ((*(uint *)&DAT_4125_0002 & 0x1f) != 0) {
      iVar10 = iVar10 + 1;
    }
    piVar8 = (int *)&DAT_4125_3292;
    bVar4 = 0;
    while( true ) {
      iVar5 = *piVar8;
      if (iVar5 == 0) {
        return uStack_4;
      }
      if (iVar10 <= *(int *)(iVar5 + 0xc)) break;
      piVar8 = piVar8 + 1;
      bVar4 = bVar4 + 1;
    }
    (*(code *)*(undefined2 *)(iVar5 + 2))();
    uVar3 = DAT_4125_3274;
    uVar1 = *(undefined2 *)&DAT_4125_0002;
    puVar2 = (undefined2 *)*(int *)s_Borland_C_____Copyright_1991_Bor_4125_0004;
    *(byte *)(puVar2 + 2) = *(byte *)(puVar2 + 2) | 8;
    *(byte *)(puVar2 + 2) = *(byte *)(puVar2 + 2) & 0xf8;
    *(byte *)(puVar2 + 2) = *(byte *)(puVar2 + 2) | bVar4;
    *puVar2 = unaff_BP;
    puVar2[1] = uVar1;
    FUN_1d28_060d();
    in_AX = uStack_4;
    if (uVar9 == 0) {
      return uStack_4;
    }
  } while( true );
}


