
undefined2 __cdecl16far FUN_6087_1637(int param_1,int param_2)

{
  int x1;
  int y1;
  int iVar1;
  int iVar2;
  int iVar3;
  
  x1 = *(int *)(param_1 * 0x1f + 0x811);
  y1 = *(int *)(param_1 * 0x1f + 0x813);
  iVar1 = map_distance(x1,y1,*(int *)(param_2 * 0x16 + -0x7780),*(int *)(param_2 * 0x16 + -0x777e));
  iVar2 = game_turn();
  iVar3 = iVar2 * 2 + 3;
  if (*(int *)(param_1 * 0x1f + 0x82c) != 0) {
    iVar3 = iVar2 * 2 + 0xf;
  }
  if ((((iVar1 < iVar3) && (iVar1 = FUN_623c_16ae(x1,y1), iVar1 != 0)) &&
      (*(char *)(*(byte *)(y1 * 0xe0 + x1 * 2) + 0x710) == '\v')) &&
     ((*(char *)(param_1 * 0x1f + 0x829) == '\x01' ||
      ((*(byte *)(y1 * 0xe0 + x1 * 2 + 1) & 0x40) == 0)))) {
    return 1;
  }
  return 0;
}


