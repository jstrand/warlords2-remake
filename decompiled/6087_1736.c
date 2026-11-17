
undefined2 __cdecl16far FUN_6087_1736(int param_1)

{
  int y;
  int iVar1;
  int iVar2;
  
  iVar2 = *(int *)(param_1 * 0x1d + 0xd02);
  y = *(int *)(param_1 * 0x1d + 0xd04);
  if (((*(char *)(param_1 * 0x1d + 0xcff) == '\x01') && (iVar1 = FUN_623c_16ae(iVar2,y), iVar1 != 0)
      ) && ((*(char *)(*(byte *)(y * 0xe0 + iVar2 * 2) + 0x710) != '\n' ||
            (iVar2 = city_at(iVar2,y), *(char *)(iVar2 * 0x41 + 0x1592) == current_player)))) {
    return 1;
  }
  return 0;
}


