
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_load_game_2(int param_1)

{
  undefined2 unaff_SS;
  char *fmt;
  undefined4 uVar1;
  char local_54 [80];
  int local_4;
  
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
    _LAB_4125_128d_1 = param_1;
    _LAB_4125_128b_1 = param_1;
    local_4 = auto_file_save_any_dat();
    build_army_move_flags();
    if ((DAT_4125_2a86 != 0) && (local_4 != 0)) {
      FUN_8065_0a6e();
      FUN_8961_0698();
    }
    if ((DAT_4125_28c6 != 0) || (DAT_4125_2a86 == 0)) {
      auto_str_cannot_allocate_vbox_savebm();
    }
    FUN_828e_03f9();
    compute_upkeep();
    compute_income();
    FUN_66d4_09f2();
    refresh_map_owners();
    path_build_cost_grid(current_player,1);
    game_screen_start(DAT_4125_28c6,0);
    DAT_4125_28c6 = 0;
    draw_turn_strip(1);
    draw_turn_strip(0);
    uVar1 = CONCAT22(0x3c04,current_player * 0x14);
    fmt = get_string(0x2f,1);
    sprintf((char *)CONCAT22(unaff_SS,local_54),fmt,uVar1);
    FUN_8065_10fb(local_54);
    FUN_8611_006c();
    FUN_828e_0af3();
    music_cue(1);
  }
  return;
}


