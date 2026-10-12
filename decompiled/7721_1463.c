
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_file_save_any_dat_2(char *param_1)

{
  undefined2 unaff_SS;
  char *pcVar1;
  char local_52 [40];
  char local_2a [40];
  
  ((undefined1 *)&DAT_451b_0712)[_LAB_4125_128b_1] = 1;
  strcpy((char *)CONCAT22(0x4125,(char *)(_LAB_4125_128b_1 * 0xf + 0x467c)),param_1);
  auto_file_mapinfo_dat_2();
  pcVar1 = get_file_string(0x1a,_LAB_4125_128b_1);
  strcpy((char *)CONCAT22(unaff_SS,local_2a),pcVar1);
  pcVar1 = get_file_string(0x1b,0);
  strcpy((char *)CONCAT22(unaff_SS,local_52),pcVar1);
  FUN_7721_0d61(local_52);
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
  return;
}


