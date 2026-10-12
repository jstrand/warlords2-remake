
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far end_turn(void)

{
  int iVar1;
  int iVar2;
  
  if (game_won == 0) {
    diplomacy_score_update();
    FUN_22bf_07a5();
    FUN_8065_0418();
    FUN_8611_0e68();
    _DAT_451b_1efe = 0;
    DAT_451b_1f02 = 0;
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar1 * 4) = 0;
      ((undefined2 *)&DAT_451b_1ede)[iVar1 * 2] = 0;
    }
    FUN_828e_0af3();
    FUN_8611_006c();
    FUN_8065_0aeb();
    iVar1 = 0;
    for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
      if ((*(int *)(iVar2 * 2 + 0x137) != 0) && (*(int *)(iVar2 * 2 + 0xd0) == 0)) {
        iVar1 = iVar1 + 1;
      }
    }
    if ((1 < iVar1) && (opt_hidden_map != 0)) {
      FUN_8065_0a6e();
    }
    FUN_22bf_0757();
    next_side();
    turn_loop();
  }
  return;
}


