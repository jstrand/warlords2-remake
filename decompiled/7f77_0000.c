
void __cdecl16far FUN_7f77_0000(int param_1)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_SS;
  undefined1 local_58 [20];
  char local_44 [64];
  int local_4;
  
  FUN_22bf_07a5();
  FUN_8cc6_092f();
  iVar1 = strcmp((char *)0x41255da0,s_random_4125_2a67);
  if (iVar1 == 0) {
    DAT_4125_28c8 = 1;
  }
  DAT_4125_28c6 = 1;
  auto_str_join_data_not_found();
  if (param_1 != 0) {
    auto_file_standard_pal();
    auto_err_error_cannot_open_file();
    if ((DAT_4125_28c8 == 0) && (DAT_4125_2a0c == 0)) {
      DAT_4125_2a0c = 1;
      iVar1 = 0;
      local_4 = 0;
      while (local_4 == 0) {
        iVar2 = auto_file_scenario_dat();
        if (iVar2 == 0) {
          local_4 = 1;
          DAT_4125_28ce = 0;
          auto_file_scenario_dat();
          __F_SCOPY(local_58,unaff_SS,0x5d64,0x4125);
        }
        else {
          iVar2 = strcmp((char *)CONCAT22(unaff_SS,local_44),s_Erythea_4125_2a6e);
          if (iVar2 == 0) {
            DAT_4125_28ce = iVar1;
            __F_SCOPY(local_58,unaff_SS,0x5d64,0x4125);
            break;
          }
          iVar1 = iVar1 + 1;
        }
      }
    }
  }
  auto_ui_random_map_terrain();
  auto_ui_random_world();
  FUN_7f77_011e();
  music_cue(0);
  if (param_1 != 0) {
    advisor_speak(2);
  }
  DAT_4125_166a = 0;
  FUN_22bf_0757();
  return;
}


