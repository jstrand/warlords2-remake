
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far hero_offer_check(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int local_6;
  
  iVar3 = 0;
  iVar4 = 0;
  _LAB_4125_110f_1 = 0;
  if (surrender_offered == 0) {
    iVar1 = game_turn();
    if (iVar1 == 1) {
      DAT_451b_058a = *(undefined2 *)(current_player * 0x14 + 0x189);
      DAT_451b_0588 = *(undefined2 *)(current_player * 0x14 + 0x18b);
      DAT_451b_058c = FUN_828e_044a(DAT_451b_058a,DAT_451b_0588);
      return 1;
    }
    local_6 = 5;
    FUN_828e_03f9();
    if (0x27 < (int)((undefined2 *)&side_city_count)[current_player]) {
      local_6 = 6;
    }
    DAT_451b_0586 = 0;
    iVar1 = army_count;
    while (iVar2 = iVar1 + -1, iVar1 != 0) {
      iVar1 = iVar2;
      if (*(char *)(iVar2 * 0x16 + -0x777c) == '\x1c') {
        iVar3 = iVar3 + 1;
        if (*(char *)(iVar2 * 0x16 + -0x777b) == (char)current_player) {
          DAT_451b_0586 = 1;
          iVar4 = iVar4 + 1;
        }
      }
    }
    if ((iVar3 < 0x28) && (iVar4 < local_6)) {
      if (DAT_451b_0586 == 0) {
        _LAB_4125_110f_1 = dice(1,400,300);
      }
      else {
        _LAB_4125_110f_1 = dice(1,600,1000);
      }
      if ((_LAB_4125_110f_1 <= *(int *)(current_player * 0x14 + 0x185)) &&
         (iVar3 = dice(1,0x1e,0), iVar3 < 7)) {
        iVar3 = -1;
        iVar1 = dice(1,((undefined2 *)&side_city_count)[current_player],0);
        iVar2 = 0;
        iVar4 = 1;
        while ((iVar2 < city_count &&
               ((iVar5 = iVar4, *(char *)(iVar2 * 0x41 + 0x1592) != (char)current_player ||
                (iVar5 = iVar4 + 1, iVar3 = iVar2, iVar4 != iVar1))))) {
          iVar2 = iVar2 + 1;
          iVar4 = iVar5;
        }
        if (iVar3 != -1) {
          DAT_451b_058c = iVar3;
          iVar4 = is_computer_turn();
          if (iVar4 == 0) {
            DAT_451b_058c = iVar3;
            return 1;
          }
          DAT_451b_058c = ai_hero_city();
          return 1;
        }
      }
    }
  }
  return 0;
}


