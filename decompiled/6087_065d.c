
void __cdecl16far FUN_6087_065d(undefined4 param_1)

{
  int *piVar1;
  int y;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  
  iVar6 = 0;
  iVar3 = 6;
  while( true ) {
    iVar4 = (int)param_1;
    uVar8 = (undefined2)((ulong)param_1 >> 0x10);
    if (iVar3 == 0) break;
    *(undefined2 *)(iVar4 + (iVar3 + -1) * 2) = 0xffff;
    iVar3 = iVar3 + -1;
  }
  *(undefined2 *)((int)DAT_451b_018e + 0xe) = 0;
  iVar3 = army_count;
  while (iVar7 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar7;
    if (((*(char *)(iVar7 * 0x16 + -0x777b) == current_player) &&
        (*(char *)(iVar7 * 0x16 + -0x777c) == '\x1c')) && (iVar6 < 6)) {
      *(int *)(iVar4 + iVar6 * 2) = iVar7;
      iVar2 = *(int *)(iVar7 * 0x16 + -0x7780);
      y = *(int *)(iVar7 * 0x16 + -0x777e);
      iVar5 = iVar4 + iVar6 * 2;
      *(undefined2 *)(iVar5 + 0x18) = 0xffff;
      *(undefined2 *)(iVar5 + 0xc) = 0xffff;
      *(undefined2 *)(iVar5 + 0x24) = 0;
      if (*(char *)(*(byte *)(y * 0xe0 + iVar2 * 2) + 0x710) == '\n') {
        iVar2 = city_at(iVar2,y);
        *(int *)(iVar4 + iVar6 * 2 + 0xc) = iVar2;
      }
      else if (*(char *)(*(byte *)(y * 0xe0 + iVar2 * 2) + 0x710) == '\v') {
        iVar2 = site_at(iVar2,y);
        *(int *)(iVar4 + iVar6 * 2 + 0x18) = iVar2;
      }
      iVar7 = hero_flight_items(iVar7 * 0x16 + -0x7780,0x2e1d);
      if (iVar7 != 0) {
        *(undefined2 *)(iVar4 + iVar6 * 2 + 0x24) = 1;
      }
      iVar6 = iVar6 + 1;
      piVar1 = (int *)((int)DAT_451b_018e + 0xe);
      *piVar1 = *piVar1 + 1;
    }
  }
  return;
}


