
void __cdecl16far FUN_5f19_0aed(void)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  iVar1 = city_count;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if ((*(char *)(iVar2 * 0x41 + 0x1592) == current_player) &&
       (uVar3 = (undefined2)((ulong)DAT_451b_018e >> 0x10),
       *(char *)((int)DAT_451b_018e + iVar2 + 0x56) == '\x06')) {
      *(undefined1 *)((int)DAT_451b_018e + iVar2 + 0x56) = 8;
      *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + iVar2 * 0x41) = 0;
    }
  }
  iVar1 = 4;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if (*(int *)((int)DAT_451b_018e + iVar2 * 0x5c + 0x24c) != 0) {
      FUN_5f19_0b72(iVar2);
    }
  }
  return;
}


