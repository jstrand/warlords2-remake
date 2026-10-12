
void __cdecl16far FUN_1b62_0d85(void)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined2 unaff_SS;
  int local_200 [255];
  
  for (iVar2 = 2; iVar1 = army_count, iVar2 < 0xff; iVar2 = iVar2 + 1) {
    local_200[iVar2] = 0;
  }
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if (((*(char *)(iVar2 * 0x16 + -0x777b) == current_player) &&
        (-1 < *(int *)(iVar2 * 0x16 + -0x7780))) && (1 < *(byte *)(iVar2 * 0x16 + -0x776f))) {
      local_200[*(byte *)(iVar2 * 0x16 + -0x776f)] =
           local_200[*(byte *)(iVar2 * 0x16 + -0x776f)] + 1;
    }
  }
  for (uVar3 = 2; (int)uVar3 < 0xff; uVar3 = uVar3 + 1) {
    iVar2 = army_count;
    if (local_200[uVar3] == 1) {
      while (iVar1 = iVar2 + -1, iVar2 != 0) {
        iVar2 = iVar1;
        if (((*(char *)(iVar1 * 0x16 + -0x777b) == current_player) &&
            (-1 < *(int *)(iVar1 * 0x16 + -0x7780))) && (*(byte *)(iVar1 * 0x16 + -0x776f) == uVar3)
           ) {
          *(undefined1 *)(iVar1 * 0x16 + -0x776f) = 0;
        }
      }
    }
  }
  return;
}


