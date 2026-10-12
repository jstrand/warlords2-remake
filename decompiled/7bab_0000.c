
void __cdecl16far auto_ui_options_menu(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  char *pcVar2;
  
  FUN_22bf_07a5();
  if (param_2 != 0) {
    FUN_1997_0066();
    if (param_4 == 2) {
      DAT_4125_28c8 = DAT_3c04_0120;
    }
    auto_err_error_cannot_open_file();
    FUN_2372_132f();
  }
  music_cue(0);
  if (param_4 != 1) {
    FUN_54f6_0000();
  }
  if ((DAT_4125_28c8 == 0) || (param_1 == 0)) {
    if (param_3 != 0) {
      auto_file_x_scn_2();
    }
  }
  else if (param_3 != 0) {
    random_map_setup();
  }
  DAT_3c04_0120 = DAT_4125_28c8;
  if (param_4 != 1) {
    FUN_54f6_069c();
  }
  auto_file_standard_pal();
  if (opt_tutorial == 0) {
    if (param_4 != 1) {
      auto_file_options_dat();
    }
    if (param_2 != 0) {
      FUN_8065_0a10(0);
      FUN_78a8_06ae(0xf0002,0x30000);
      FUN_7bab_0634(0xffff);
      difficulty_rating();
      FUN_7bab_10c7();
      auto_file_quotes_dat();
    }
    DAT_4125_166a = 0;
    if (param_2 != 0) {
      pcVar2 = get_string(7,0);
      FUN_7ecb_00d6(0x300200,(char *)pcVar2,(int)((ulong)pcVar2 >> 0x10));
      pcVar2 = get_string(7,1);
      FUN_7ecb_00d6(0xce0200,(char *)pcVar2,(int)((ulong)pcVar2 >> 0x10));
    }
    auto_str_join_data_not_found();
    FUN_7bab_0416();
    FUN_22bf_0757();
  }
  else {
    for (iVar1 = 0; iVar1 < 10; iVar1 = iVar1 + 1) {
      ((undefined2 *)&DAT_4125_23b4)[iVar1] = ((undefined2 *)&DAT_4125_2378)[iVar1];
    }
    DAT_4125_166a = 0;
    FUN_22bf_0757();
    pcVar2 = get_file_string(0x19,8);
    FUN_8065_168d((char *)pcVar2,(int)((ulong)pcVar2 >> 0x10));
    start_game_from_setup();
  }
  return;
}


