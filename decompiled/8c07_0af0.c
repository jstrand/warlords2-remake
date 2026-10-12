
bool __cdecl16far FUN_8c07_0af0(void)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  char *pcVar7;
  int iVar8;
  char *local_4;
  
  local_4 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  iVar3 = army_count;
  iVar8 = -1;
  while (iVar4 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar4;
    if ((((*(char *)(iVar4 * 0x16 + -0x777b) == (char)current_player) &&
         (-1 < *(int *)(iVar4 * 0x16 + -0x7780))) && ((*(uint *)(iVar4 * 0x16 + -0x7774) & 1) != 0))
       && ((*(uint *)(iVar4 * 0x16 + -0x7774) & 0x40) == 0)) {
      uVar1 = *(int *)(iVar4 * 0x16 + -0x7780) - DAT_4125_3250;
      uVar5 = (int)uVar1 >> 0xf;
      uVar2 = *(int *)(iVar4 * 0x16 + -0x777e) - DAT_4125_3252;
      uVar6 = (int)uVar2 >> 0xf;
      pcVar7 = (char *)(((uVar1 ^ uVar5) - uVar5) + ((uVar2 ^ uVar6) - uVar6));
      if (pcVar7 == (char *)0x0) {
        pcVar7 = (char *)0x2328;
      }
      if (((*(uint *)(iVar4 * 0x16 + -0x7774) & 0x200) == 0) && ((int)pcVar7 < (int)local_4)) {
        iVar8 = iVar4;
        local_4 = pcVar7;
      }
    }
  }
  return iVar8 != -1;
}


