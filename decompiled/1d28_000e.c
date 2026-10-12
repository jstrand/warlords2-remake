
int __cdecl16far FUN_1d28_000e(int param_1,int param_2)

{
  undefined2 *puVar1;
  code *pcVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  undefined2 *puVar8;
  undefined2 unaff_SS;
  bool bVar9;
  undefined4 uVar10;
  
  DAT_4125_3288 = (int *)&DAT_4125_3292;
  iVar7 = 10;
  while( true ) {
    iVar4 = *(int *)(&stack0xfffe + iVar7);
    if ((iVar4 == -1) || (param_2 == 0)) break;
    bVar9 = iVar4 < 0;
    iVar4 = *(int *)(iVar4 * 2 + 0x328a);
    (*(code *)*(undefined2 *)(iVar4 + 8))();
    if (!bVar9) {
      (*(code *)*(undefined2 *)(iVar4 + 4))();
      if (!bVar9) {
        param_2 = param_2 - (*(uint *)(iVar4 + 0xc) >> 5);
        *DAT_4125_3288 = iVar4;
        DAT_4125_3288 = DAT_4125_3288 + 1;
      }
    }
    iVar7 = iVar7 + 2;
  }
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  pcVar2 = (code *)swi(0x21);
  uVar10 = (*pcVar2)();
  DAT_4125_3274 = (int)uVar10;
  DAT_4125_327e = DAT_4125_3274 + (int)((ulong)uVar10 >> 0x10);
  uVar3 = (param_1 * 5 | 0xfU) + 1 >> 4;
  iVar7 = uVar3 + DAT_4125_3274;
  for (piVar6 = (int *)&DAT_4125_3292; *piVar6 != 0; piVar6 = piVar6 + 1) {
    iVar7 = FUN_1d28_0b89();
  }
  iVar7 = iVar7 + 1;
  DAT_4125_327a = iVar7;
  DAT_4125_327c = iVar7;
  iVar4 = FUN_1d28_068c();
  iVar4 = DAT_4125_327e - iVar4;
  *(int *)&DAT_4125_0002 = iVar4;
  iVar7 = DAT_4125_3274;
  puVar8 = (undefined2 *)0x0;
  for (iVar5 = uVar3 << 3; iVar5 != 0; iVar5 = iVar5 + -1) {
    puVar1 = puVar8;
    puVar8 = puVar8 + 1;
    *puVar1 = 0;
  }
  DAT_4125_3278 = (int)puVar8 - (uint)puVar8 % 5;
  *(char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 = -0x60;
  return (iVar4 + -1) * 0x10;
}


