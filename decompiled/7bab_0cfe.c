
void __cdecl16far start_game_from_setup(void)

{
  int iVar1;
  int iVar2;
  
  iVar1 = 0;
  for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
    if ((*(int *)(iVar2 * 2 + 0x137) != 0) && (*(int *)(iVar2 * 2 + 0xc0) != 3)) {
      iVar1 = iVar1 + 1;
    }
  }
  if (iVar1 < 1) {
    sound_chord();
    FUN_7bab_0416();
  }
  else {
    auto_file_options_dat_2();
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      if (*(int *)(iVar1 * 2 + 0x137) == 0) {
        *(undefined2 *)(iVar1 * 2 + 0xd0) = 1;
        *(undefined2 *)(iVar1 * 2 + 0xc0) = 0;
      }
      if (*(int *)(iVar1 * 2 + 0xc0) == 3) {
        *(undefined2 *)(iVar1 * 2 + 0x137) = 0;
        *(undefined2 *)(iVar1 * 2 + 0xd0) = 1;
        *(undefined2 *)(iVar1 * 2 + 0xc0) = 0;
        iVar2 = FUN_828e_044a(*(undefined2 *)(iVar1 * 0x14 + 0x189));
        if (-1 < iVar2) {
          *(undefined1 *)(iVar2 * 0x41 + 0x1592) = 0xf;
        }
      }
      if (*(int *)(iVar1 * 2 + 0xd0) == 0) {
        *(undefined2 *)(iVar1 * 2 + 0xc0) = 2;
      }
    }
    music_cue(0xb);
    advisor_speak(1);
    opt_neutral_cities = DAT_4125_23b4;
    opt_diplomacy = DAT_4125_23b6;
    opt_quests = DAT_4125_23b8;
    opt_hidden_map = DAT_4125_23ba;
    opt_view_enemies = DAT_4125_23bc;
    opt_view_production = DAT_4125_23be;
    opt_intense_combat = DAT_4125_23c0;
    opt_quick_start = DAT_4125_23c2;
    opt_military_advisor = DAT_4125_23c4;
    opt_random_turns = DAT_4125_23c6;
    DAT_3c04_0130 = 0;
    FUN_5db9_0519();
    FUN_8065_0aeb();
    if (opt_hidden_map != 0) {
      FUN_8065_0a6e();
    }
    game_screen_start(0x10000);
  }
  return;
}


