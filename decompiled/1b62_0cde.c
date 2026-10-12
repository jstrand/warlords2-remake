
int __cdecl16far FUN_1b62_0cde(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_SS;
  int local_200 [255];
  
  for (iVar2 = 2; iVar1 = army_count, iVar2 < 0xff; iVar2 = iVar2 + 1) {
    local_200[iVar2] = 0;
  }
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if ((*(char *)(iVar2 * 0x16 + -0x777b) == current_player) &&
       (-1 < *(int *)(iVar2 * 0x16 + -0x7780))) {
      local_200[*(byte *)(iVar2 * 0x16 + -0x776f)] = 1;
    }
  }
  iVar2 = 1;
  while( true ) {
    if (0xfe < iVar2) {
      return 0;
    }
    if (local_200[iVar2] == 0) break;
    iVar2 = iVar2 + 1;
  }
  return iVar2;
}


