
void __cdecl16far FUN_5e97_038d(undefined2 param_1,undefined2 param_2)

{
  int *piVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  
  bVar2 = false;
  if ((opt_quests != 0) && (*(int *)(current_player * 0xc + 0x1103) != 0)) {
    iVar3 = (int)combat_atk_count;
    while (iVar4 = iVar3 + -1, iVar3 != 0) {
      iVar3 = iVar4;
      if ((((undefined2 *)&DAT_451b_0368)[iVar4 * 2] == 0x2e1d) &&
         (*(int *)(current_player * 0xc + 0x1107) * 0x16 + -0x7780 ==
          ((undefined2 *)&combat_atk_armies)[iVar4 * 2])) {
        bVar2 = true;
      }
    }
    if ((bVar2) &&
       (((*(int *)(current_player * 0xc + 0x1105) == 4 ||
         (*(int *)(current_player * 0xc + 0x1105) == 5)) &&
        (iVar3 = FUN_828e_044a(param_1,param_2), *(int *)(current_player * 0xc + 0x1109) == iVar3)))
       ) {
      uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      iVar4 = (int)DAT_451b_018e;
      piVar1 = (int *)(iVar4 + 0x4a);
      *piVar1 = *piVar1 + 1;
      *(undefined2 *)(iVar4 + 0x46) = 0xffff;
      if (*(int *)(current_player * 0xc + 0x1105) == 5) {
        auto_str_is_being_razed(iVar3,1);
        uVar5 = 2;
      }
      else {
        uVar5 = 4;
      }
      quest_check(uVar5,0,0);
    }
  }
  return;
}


