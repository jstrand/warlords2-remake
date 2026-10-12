
void __cdecl16far FUN_8cc6_089d(void)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = army_count;
  while (iVar4 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar4;
    if ((*(char *)(iVar4 * 0x16 + -0x777b) == current_player) &&
       (*(int *)(iVar4 * 0x16 + -0x7774) != 0)) {
      iVar2 = FUN_828e_044a(*(undefined2 *)(iVar4 * 0x16 + -0x7780),
                            *(undefined2 *)(iVar4 * 0x16 + -0x777e));
      if (-1 < iVar2) {
        puVar1 = (uint *)(iVar4 * 0x16 + -0x7774);
        *puVar1 = *puVar1 & 0xefff;
      }
    }
  }
  return;
}


