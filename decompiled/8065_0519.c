
void __cdecl16far FUN_8065_0519(void)

{
  char cVar1;
  bool bVar2;
  bool bVar3;
  char cVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  bool bVar10;
  bool bVar11;
  
  bVar2 = false;
  bVar3 = false;
  bVar11 = false;
  bVar10 = DAT_451b_1f02 != (int *)0x0;
  uVar9 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
  if (bVar10) {
    iVar6 = *DAT_451b_1f02;
    iVar5 = ((int *)DAT_451b_1f02)[1];
  }
  else {
    iVar6 = 0;
    iVar5 = 0;
  }
  cVar1 = *(char *)(*(byte *)(iVar5 * 0xe0 + iVar6 * 2) + 0x710);
  iVar8 = army_count;
  if (bVar10) {
    cVar4 = (char)((int *)DAT_451b_1f02)[2];
  }
  else {
    cVar4 = -1;
  }
  do {
    iVar7 = iVar8 + -1;
    if (iVar8 == 0) goto LAB_8065_05ec;
    iVar8 = iVar7;
  } while ((*(char *)(iVar7 * 0x16 + -0x777c) != '\x1c') ||
          (*(char *)(iVar7 * 0x16 + -0x777b) != current_player));
  bVar2 = true;
LAB_8065_05ec:
  if (cVar1 == '\v') {
    for (iVar8 = 0; iVar8 < site_count; iVar8 = iVar8 + 1) {
      if ((*(int *)(iVar8 * 0x1f + 0x811) == iVar6) && (*(int *)(iVar8 * 0x1f + 0x813) == iVar5)) {
        bVar3 = true;
        bVar11 = *(char *)(iVar8 * 0x1f + 0x829) == '\x01';
        break;
      }
    }
  }
  FUN_2372_0eab(0x10000);
  FUN_2372_0eab(0x10001);
  FUN_2372_0eab(0x10002);
  FUN_2372_0eab(0x10003);
  FUN_2372_0eab(0x10004);
  FUN_2372_0eab(0x10005);
  FUN_2372_0eab(0x10006);
  FUN_2372_0eab(0x10007);
  iVar8 = FUN_7721_0e25();
  if (iVar8 == 0) {
    FUN_2372_0ef8(0x1f6);
  }
  iVar8 = FUN_7721_0e42();
  if (iVar8 == 0) {
    FUN_2372_0ef8(0x1f8);
  }
  if ((DAT_3c04_0120 == 0) || (DAT_3c04_0130 != 0)) {
    FUN_2372_0ef8(0x1f7);
  }
  if (bVar10) {
    if (cVar1 != '\t') goto LAB_8065_072b;
  }
  else {
    FUN_2372_0ef8(0x1fc);
LAB_8065_072b:
    FUN_2372_0ef8(0x207);
  }
  if (((undefined2 *)&side_city_count)[current_player] == 0) {
    FUN_2372_0ef8(0x1fd);
  }
  if (opt_diplomacy == 0) {
    FUN_2372_0ef8(0x203);
  }
  if (!bVar2) {
    FUN_2372_0ef8(0x205);
    FUN_2372_0ef8(0x208);
  }
  if (((bVar10) && (cVar4 == '\x1c')) && (*(char *)(current_player * 0x1d + 0xcff) == '\x03')) {
    if ((((DAT_451b_1f02._2_2_ == 0x2e1d) &&
         ((int *)(*(int *)(current_player * 0x1d + 0xd00) * 0x16 + -0x7780) == (int *)DAT_451b_1f02)
         ) && (((*(byte *)(iVar5 * 0x70 + iVar6) & 0x40) == 0 &&
               ((cVar1 != '\n' && (cVar1 != '\v')))))) && ((cVar1 != '\x02' && (cVar1 != '\x03'))))
    goto LAB_8065_082f;
  }
  FUN_2372_0ef8(0x206);
LAB_8065_082f:
  if (((!bVar3) || (!bVar10)) ||
     (((cVar4 != '\x1c' && (!bVar11)) ||
      ((cVar1 != '\v' || ((*(byte *)(iVar5 * 0xe0 + iVar6 * 2 + 1) & 0x40) != 0)))))) {
    FUN_2372_0ef8(0x209);
  }
  if ((int)((undefined2 *)&side_city_count)[current_player] < 1) {
    FUN_2372_0ef8(0x20c);
    FUN_2372_0ef8(0x20e);
    FUN_2372_0ef8(0x20f);
  }
  if (!bVar10) {
    FUN_2372_0ef8(0x211);
  }
  iVar6 = game_turn();
  if (iVar6 == 1) {
    FUN_2372_0ef8(0x212);
    FUN_2372_0ef8(0x213);
    FUN_2372_0ef8(0x214);
    FUN_2372_0ef8(0x215);
  }
  if (game_won != 0) {
    FUN_2372_0ef8(0x217);
  }
  return;
}


