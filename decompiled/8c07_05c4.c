
int __cdecl16far FUN_8c07_05c4(void)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  char *pcVar8;
  char *local_4;
  
  local_4 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  iVar3 = army_count;
  iVar7 = -1;
  while (iVar4 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar4;
    if ((((*(char *)(iVar4 * 0x16 + -0x777b) == (char)current_player) &&
         (-1 < *(int *)(iVar4 * 0x16 + -0x7780))) && ((*(uint *)(iVar4 * 0x16 + -0x7774) & 1) != 0))
       && ((*(uint *)(iVar4 * 0x16 + -0x7774) & 0x40) == 0)) {
      uVar1 = *(int *)(iVar4 * 0x16 + -0x7780) - DAT_4125_3250;
      uVar5 = (int)uVar1 >> 0xf;
      uVar2 = *(int *)(iVar4 * 0x16 + -0x777e) - DAT_4125_3252;
      uVar6 = (int)uVar2 >> 0xf;
      pcVar8 = (char *)(((uVar1 ^ uVar5) - uVar5) + ((uVar2 ^ uVar6) - uVar6));
      if (pcVar8 == (char *)0x0) {
        pcVar8 = (char *)0x2328;
      }
      if (((*(uint *)(iVar4 * 0x16 + -0x7774) & 0x200) == 0) && ((int)pcVar8 < (int)local_4)) {
        iVar7 = iVar4;
        local_4 = pcVar8;
      }
    }
  }
  if (iVar7 == -1) {
    iVar3 = 0;
  }
  else {
    DAT_4125_3250 = *(int *)(iVar7 * 0x16 + -0x7780);
    DAT_4125_3252 = *(int *)(iVar7 * 0x16 + -0x777e);
    iVar3 = iVar7 * 0x16 + -0x7780;
  }
  return iVar3;
}


