
undefined2 __cdecl16far FUN_6087_1736(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = *(int *)(param_1 * 0x1d + 0xd02);
  iVar1 = *(int *)(param_1 * 0x1d + 0xd04);
  if (((*(char *)(param_1 * 0x1d + 0xcff) == '\x01') &&
      (iVar2 = FUN_623c_16ae(iVar3,iVar1), iVar2 != 0)) &&
     ((*(char *)(*(byte *)(iVar1 * 0xe0 + iVar3 * 2) + 0x710) != '\n' ||
      (iVar3 = FUN_828e_044a(iVar3,iVar1), *(char *)(iVar3 * 0x41 + 0x1592) == current_player)))) {
    return 1;
  }
  return 0;
}


