
void __cdecl16far FUN_6087_065d(undefined4 param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined2 uVar10;
  
  iVar8 = 0;
  iVar5 = 6;
  while( true ) {
    iVar6 = (int)param_1;
    uVar10 = (undefined2)((ulong)param_1 >> 0x10);
    if (iVar5 == 0) break;
    *(undefined2 *)(iVar6 + (iVar5 + -1) * 2) = 0xffff;
    iVar5 = iVar5 + -1;
  }
  *(undefined2 *)((int)DAT_451b_018e + 0xe) = 0;
  iVar5 = army_count;
  while (iVar9 = iVar5 + -1, iVar5 != 0) {
    iVar5 = iVar9;
    if (((*(char *)(iVar9 * 0x16 + -0x777b) == current_player) &&
        (*(char *)(iVar9 * 0x16 + -0x777c) == '\x1c')) && (iVar8 < 6)) {
      *(int *)(iVar6 + iVar8 * 2) = iVar9;
      iVar2 = *(int *)(iVar9 * 0x16 + -0x7780);
      iVar3 = *(int *)(iVar9 * 0x16 + -0x777e);
      iVar7 = iVar6 + iVar8 * 2;
      *(undefined2 *)(iVar7 + 0x18) = 0xffff;
      *(undefined2 *)(iVar7 + 0xc) = 0xffff;
      *(undefined2 *)(iVar7 + 0x24) = 0;
      if (*(char *)(*(byte *)(iVar3 * 0xe0 + iVar2 * 2) + 0x710) == '\n') {
        uVar4 = FUN_828e_044a(iVar2,iVar3);
        *(undefined2 *)(iVar6 + iVar8 * 2 + 0xc) = uVar4;
      }
      else if (*(char *)(*(byte *)(iVar3 * 0xe0 + iVar2 * 2) + 0x710) == '\v') {
        uVar4 = FUN_828e_04b5(iVar2,iVar3);
        *(undefined2 *)(iVar6 + iVar8 * 2 + 0x18) = uVar4;
      }
      iVar9 = hero_flight_items(iVar9 * 0x16 + -0x7780,0x2e1d);
      if (iVar9 != 0) {
        *(undefined2 *)(iVar6 + iVar8 * 2 + 0x24) = 1;
      }
      iVar8 = iVar8 + 1;
      piVar1 = (int *)((int)DAT_451b_018e + 0xe);
      *piVar1 = *piVar1 + 1;
    }
  }
  return;
}


