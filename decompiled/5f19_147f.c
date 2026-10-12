
undefined2 __cdecl16far FUN_5f19_147f(void)

{
  int iVar1;
  int unaff_BP;
  int unaff_SI;
  int unaff_DI;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  bool bVar5;
  
  do {
    if (((*(int *)(unaff_DI * 2 + 0x137) != 0) && (*(int *)(unaff_DI * 2 + 0xd0) != 0)) &&
       ((*(int *)(current_player * 2 + unaff_BP + -0xce) != 0 ||
        (*(int *)(unaff_DI * 2 + unaff_BP + -0xce) != 0)))) {
      *(undefined2 *)(unaff_DI * 2 + unaff_BP + -0xbe) = 0;
    }
    bVar5 = unaff_DI != 0;
    unaff_DI = unaff_DI + -1;
  } while (bVar5);
  iVar1 = 8;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar1 = (int)DAT_451b_018e + iVar2 * 2;
    *(int *)(unaff_BP + -0x1c) = *(int *)(iVar1 + 0x3dc) + *(int *)(iVar1 + 0x3fc);
    if (opt_quick_start == 0) {
      uVar4 = 8;
    }
    else {
      uVar4 = 4;
    }
    *(undefined2 *)(unaff_BP + -0x1e) = uVar4;
    iVar1 = FUN_558d_0a6e(iVar2);
    if (iVar1 != 0) {
      *(undefined2 *)(iVar2 * 2 + unaff_BP + -0xbe) = 0;
    }
    if ((unaff_SI != 0) && ((*(byte *)(current_player * 8 + iVar2 + 0x153b) & 0xc) == 0)) {
      *(undefined2 *)(iVar2 * 2 + unaff_BP + -0xbe) = 0;
    }
    if ((*(int *)(unaff_BP + -6) == 0) && (*(int *)(iVar2 * 2 + unaff_BP + -0x5e) == 0)) {
      *(undefined2 *)(iVar2 * 2 + unaff_BP + -0xbe) = 0;
    }
    iVar1 = game_turn();
    if ((iVar1 < *(int *)(unaff_BP + -0x1e)) && (*(int *)(unaff_BP + -0x1c) == 0)) {
      *(undefined2 *)(iVar2 * 2 + unaff_BP + -0xbe) = 0;
    }
    iVar1 = iVar2;
    if ((1 < *(int *)(unaff_BP + -8)) &&
       (*(int *)(iVar2 * 2 + unaff_BP + -0x2e) / 4 < *(int *)(iVar2 * 2 + unaff_BP + -0xae))) {
      *(undefined2 *)(iVar2 * 2 + unaff_BP + -0xbe) = 0;
    }
  }
  *(undefined2 *)(current_player * 2 + unaff_BP + -0xbe) = 0;
  iVar1 = 0;
  iVar2 = 8;
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    iVar2 = iVar3;
    if ((*(int *)(iVar3 * 2 + unaff_BP + -0x2e) != 0) &&
       (iVar1 < *(int *)(iVar3 * 2 + unaff_BP + -0xbe))) {
      iVar1 = *(int *)(iVar3 * 2 + unaff_BP + -0xbe);
      *(int *)(unaff_BP + -10) = iVar3;
    }
  }
  if ((*(char *)(*(int *)(unaff_BP + -0xc) * 0x41 + 0x1592) != '\x0f') &&
     (*(char *)(*(int *)(unaff_BP + -0xc) * 0x41 + 0x1592) != current_player)) {
    bVar5 = false;
    iVar1 = *(int *)((int)DAT_451b_018e + 0x24a);
    while (iVar2 = iVar1 + -1, iVar1 != 0) {
      iVar1 = iVar2;
      if ((int)*(char *)(*(int *)(unaff_BP + -0xc) * 0x41 + 0x1592) ==
          *(int *)(iVar2 * 2 + unaff_BP + -0xae)) {
        bVar5 = true;
      }
    }
    if (!bVar5) {
      *(int *)(unaff_BP + -10) = (int)*(char *)(*(int *)(unaff_BP + -0xc) * 0x41 + 0x1592);
    }
  }
  if ((*(int *)(unaff_BP + -10) != -1) &&
     (*(int *)(*(int *)(unaff_BP + -10) * 2 + unaff_BP + -0x5e) == 0)) {
    *(undefined2 *)(unaff_BP + -10) = 0xffff;
  }
  return *(undefined2 *)(unaff_BP + -10);
}


