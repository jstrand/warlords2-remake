
char * __cdecl16near __heap_grow_copy(void)

{
  undefined2 *puVar1;
  char *pcVar2;
  undefined2 *puVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  void *in_BX;
  void *pvVar10;
  char *pcVar11;
  undefined2 *puVar12;
  char *pcVar13;
  undefined2 *puVar14;
  bool bVar15;
  char *pcVar16;
  
  pvVar10 = in_BX;
  pcVar16 = farmalloc(CONCAT22(DAT_1000_319e,DAT_1000_31a0));
  iVar8 = (int)((ulong)pcVar16 >> 0x10);
  if (iVar8 == 0) {
    return pcVar16;
  }
  uVar9 = *(int *)0x0 - 1;
  pcVar13 = (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004;
  pcVar11 = (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004;
  for (iVar5 = 6; iVar5 != 0; iVar5 = iVar5 + -1) {
    pcVar2 = pcVar13;
    pcVar13 = pcVar13 + 2;
    pcVar16 = pcVar11;
    pcVar11 = pcVar11 + 2;
    *(undefined2 *)pcVar2 = *(undefined2 *)pcVar16;
  }
  if (uVar9 != 0) {
    iVar5 = iVar8 + 1;
    iVar4 = (int)in_BX + 1;
    while( true ) {
      puVar14 = (undefined2 *)0x0;
      puVar12 = (undefined2 *)0x0;
      uVar6 = uVar9;
      if (0x1000 < uVar9) {
        uVar6 = 0x1000;
      }
      for (iVar7 = uVar6 << 3; iVar7 != 0; iVar7 = iVar7 + -1) {
        puVar3 = puVar14;
        puVar14 = puVar14 + 1;
        puVar1 = puVar12;
        puVar12 = puVar12 + 1;
        *puVar3 = *puVar1;
      }
      bVar15 = uVar9 < 0x1000;
      uVar9 = uVar9 - 0x1000;
      if (bVar15 || uVar9 == 0) break;
      iVar5 = iVar5 + 0x1000;
      iVar4 = iVar4 + 0x1000;
    }
  }
  free((void *)CONCAT22(in_BX,pvVar10));
  return (char *)CONCAT22(iVar8,(char *)s_Borland_C_____Copyright_1991_Bor_4125_0004);
}


