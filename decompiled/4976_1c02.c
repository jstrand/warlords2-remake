
void __cdecl16far quest_give_reward(uint param_1,int param_2)

{
  int *piVar1;
  uint *puVar2;
  int iVar3;
  
  if (param_1 < 4) {
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)((char *)s_Quest_4125_1de3 + 2 + param_1 * 2)) {
    case 0x4b37c:
      piVar1 = (int *)(current_player * 0x14 + 0x185);
      *piVar1 = *piVar1 + param_2;
      break;
    case 0x4b396:
      *(undefined1 *)(param_2 * 0x1d + 0xcff) = 3;
      *(undefined2 *)(param_2 * 0x1d + 0xd00) = *(undefined2 *)(current_player * 0xc + 0x1107);
      break;
    case 0x4b3cf:
      puVar2 = (uint *)(param_2 * 0x1f + 0x82e);
      *puVar2 = *puVar2 | 1 << ((byte)current_player & 0x1f);
      if (((DAT_3c04_0159 != 0) && (DAT_4125_3254 == 0)) ||
         (*(int *)(current_player * 2 + 0xd0) == 0)) {
        FUN_8611_1298();
        FUN_834b_2200(current_player);
        FUN_66d4_09f2(current_player);
      }
      break;
    case 0x4b46f:
      for (iVar3 = 0; iVar3 < param_2; iVar3 = iVar3 + 1) {
        create_ally_army();
      }
    }
  }
  FUN_6d51_1244(current_player,2,0,
                *(char *)(*(int *)(current_player * 0xc + 0x1107) * 0x16 + -0x7776) * 0x14 + 0x223,
                0x3c04);
  hero_add_experience();
  return;
}


