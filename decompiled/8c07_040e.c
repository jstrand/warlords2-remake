
int __cdecl16far FUN_8c07_040e(void)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  char *pcVar9;
  char *local_8;
  char *local_6;
  int local_4;
  
  local_4 = -1;
  local_6 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  local_8 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  iVar5 = army_count;
  iVar6 = -1;
  while (iVar4 = iVar5 + -1, iVar5 != 0) {
    iVar5 = iVar4;
    if ((((*(char *)(iVar4 * 0x16 + -0x777b) == (char)current_player) &&
         (-1 < *(int *)(iVar4 * 0x16 + -0x7780))) && ((*(uint *)(iVar4 * 0x16 + -0x7774) & 1) != 0))
       && ((*(uint *)(iVar4 * 0x16 + -0x7774) & 0x40) == 0)) {
      uVar2 = *(int *)(iVar4 * 0x16 + -0x7780) - DAT_4125_3250;
      uVar7 = (int)uVar2 >> 0xf;
      uVar3 = *(int *)(iVar4 * 0x16 + -0x777e) - DAT_4125_3252;
      uVar8 = (int)uVar3 >> 0xf;
      pcVar9 = (char *)(((uVar2 ^ uVar7) - uVar7) + ((uVar3 ^ uVar8) - uVar8));
      if (pcVar9 == (char *)0x0) {
        pcVar9 = (char *)0x2328;
      }
      if ((*(uint *)(iVar4 * 0x16 + -0x7774) & 0x200) == 0) {
        if ((int)pcVar9 < (int)local_6) {
          iVar6 = iVar4;
          local_6 = pcVar9;
        }
      }
      else if ((int)pcVar9 < (int)local_8) {
        local_8 = pcVar9;
        local_4 = iVar4;
      }
    }
  }
  if (iVar6 == -1) {
    if (local_4 == -1) {
      return 0;
    }
    DAT_4125_3250 = *(undefined2 *)(local_4 * 0x16 + -0x7780);
    DAT_4125_3252 = *(undefined2 *)(local_4 * 0x16 + -0x777e);
    iVar5 = army_count;
    while (iVar6 = iVar5 + -1, iVar5 != 0) {
      iVar5 = iVar6;
      if (*(char *)(iVar6 * 0x16 + -0x777b) == (char)current_player) {
        puVar1 = (uint *)(iVar6 * 0x16 + -0x7774);
        *puVar1 = *puVar1 & 0xfdff;
      }
    }
  }
  else {
    DAT_4125_3250 = *(undefined2 *)(iVar6 * 0x16 + -0x7780);
    DAT_4125_3252 = *(undefined2 *)(iVar6 * 0x16 + -0x777e);
    local_4 = iVar6;
  }
  return local_4 * 0x16 + -0x7780;
}


