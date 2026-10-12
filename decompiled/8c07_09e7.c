
undefined2 __cdecl16far FUN_8c07_09e7(void)

{
  uint uVar1;
  uint uVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  char *pcVar8;
  int iVar9;
  char *local_8;
  char *local_6;
  int local_4;
  
  local_4 = -1;
  local_6 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  local_8 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  iVar4 = army_count;
  iVar9 = -1;
  while (iVar5 = iVar4 + -1, iVar4 != 0) {
    iVar4 = iVar5;
    if ((((*(char *)(iVar5 * 0x16 + -0x777b) == (char)current_player) &&
         (-1 < *(int *)(iVar5 * 0x16 + -0x7780))) && ((*(uint *)(iVar5 * 0x16 + -0x7774) & 1) != 0))
       && ((*(uint *)(iVar5 * 0x16 + -0x7774) & 0x40) == 0)) {
      uVar1 = *(int *)(iVar5 * 0x16 + -0x7780) - DAT_4125_3250;
      uVar6 = (int)uVar1 >> 0xf;
      uVar2 = *(int *)(iVar5 * 0x16 + -0x777e) - DAT_4125_3252;
      uVar7 = (int)uVar2 >> 0xf;
      pcVar8 = (char *)(((uVar1 ^ uVar6) - uVar6) + ((uVar2 ^ uVar7) - uVar7));
      if (pcVar8 == (char *)0x0) {
        pcVar8 = (char *)0x2328;
      }
      if ((*(uint *)(iVar5 * 0x16 + -0x7774) & 0x200) == 0) {
        if ((int)pcVar8 < (int)local_6) {
          iVar9 = iVar5;
          local_6 = pcVar8;
        }
      }
      else if ((int)pcVar8 < (int)local_8) {
        local_8 = pcVar8;
        local_4 = iVar5;
      }
    }
  }
  if ((iVar9 == -1) && (local_4 == -1)) {
    uVar3 = 0;
  }
  else {
    uVar3 = 1;
  }
  return uVar3;
}


