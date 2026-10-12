
void __cdecl16far turn_loop(void)

{
  bool bVar1;
  bool bVar2;
  int iVar3;
  
  bVar2 = false;
  do {
    if ((bVar2) || ((game_won != 0 && (DAT_3c04_0157 != 0)))) {
      return;
    }
    FUN_7ecb_0a1b();
    bVar1 = true;
    for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
      if ((*(int *)(iVar3 * 2 + 0x137) != 0) && (*(int *)(iVar3 * 2 + 0xd0) == 0)) {
        bVar1 = false;
        break;
      }
    }
    FUN_7ecb_01f1();
    if (bVar1) {
      FUN_251d_008a();
    }
    FUN_66d4_09f2(current_player);
    refresh_map_owners();
    path_build_cost_grid(current_player,1);
    diplomacy_flag_pending();
    FUN_8cc6_089d();
    FUN_67cc_10d7(current_player);
    auto_ui_all_cities_seen();
    if (*(int *)(current_player * 2 + 0xd0) == 1) {
      music_cue(2);
      DAT_4125_2ade = current_player;
      FUN_22bf_07a5();
      start_of_turn(current_player);
      FUN_22bf_0757();
      next_side();
      sound_poll();
      if (DAT_4125_166a != 0) {
        bVar2 = true;
      }
    }
    else {
      FUN_8cc6_0259(current_player);
      bVar2 = true;
      DAT_4125_2ade = current_player;
    }
  } while( true );
}


