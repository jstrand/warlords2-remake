
undefined2 __cdecl16far FUN_8c07_0113(void)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = army_count;
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    iVar2 = iVar3;
    if (*(char *)(iVar3 * 0x16 + -0x777b) == (char)current_player) {
      puVar1 = (uint *)(iVar3 * 0x16 + -0x7774);
      *puVar1 = *puVar1 & 0xffbf;
      puVar1 = (uint *)(iVar3 * 0x16 + -0x7774);
      *puVar1 = *puVar1 & 0xfdff;
    }
  }
  DAT_4125_3250 = *(undefined2 *)(current_player * 0x14 + 0x189);
  DAT_4125_3252 = *(undefined2 *)(current_player * 0x14 + 0x18b);
  return 1;
}


