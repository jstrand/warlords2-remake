
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_file_rmaptemp_dat_2(int param_1)

{
  char *pcVar1;
  char *pcVar2;
  
  if (param_1 == -1) {
    if (DAT_4125_28c6 == 0) {
      if (DAT_4125_2a86 == 0) {
        auto_ui_options_menu(0,0x10000);
      }
      else {
        game_screen_start(0);
      }
    }
    else {
      FUN_7f77_0000();
    }
  }
  else {
    _LAB_4125_128b_1 = param_1;
    _LAB_4125_128f_1 = param_1;
    auto_file_save_any_dat();
    pcVar1 = get_file_string(0x1b,0);
    pcVar2 = get_file_string(0x1a,param_1);
    FUN_7721_0d61((char *)pcVar2,(int)((ulong)pcVar2 >> 0x10),pcVar1);
    FUN_828e_03f9();
    compute_upkeep();
    compute_income();
    path_build_cost_grid(current_player,1);
    auto_ui_options_menu(0x10000,0x10000);
    DAT_4125_28c6 = 0;
    DAT_4125_2a86 = 0;
    FUN_828e_0af3();
    music_cue(0);
  }
  return;
}


