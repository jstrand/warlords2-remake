
void __cdecl16far game_screen_start(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  char *pcVar3;
  char *pcVar4;
  
  iVar2 = 0;
  if (param_1 != 0) {
    auto_err_error_cannot_open_file();
    FUN_2372_132f();
  }
  if (param_2 != 0) {
    turn_order();
    next_side();
    draw_turn_strip(0x10001);
    FUN_79fa_0000();
  }
  DAT_4125_2a86 = 1;
  DAT_4125_166a = 0;
  auto_str_join_data_not_found();
  FUN_22bf_07a5();
  if (param_2 == 0) {
    FUN_8611_006c();
    FUN_834b_0000(0x10001,0);
  }
  FUN_8065_0aeb();
  FUN_8065_0a9d();
  FUN_8065_0174();
  FUN_2372_132f();
  if (param_2 == 0) {
    FUN_89e0_0356();
  }
  FUN_22bf_0757();
  if (param_2 != 0) {
    FUN_8065_0418();
    if (opt_hidden_map == 0) {
      FUN_834b_0000(0x10001,0);
    }
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      if ((*(int *)(iVar1 * 2 + 0x137) != 0) && (*(int *)(iVar1 * 2 + 0xd0) == 0)) {
        iVar2 = iVar2 + 1;
      }
    }
    if (iVar2 == 0) {
      pcVar3 = get_string(0xe,1);
      pcVar4 = get_string(0xe,0);
      FUN_8065_1160((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10),pcVar3);
    }
    turn_loop();
  }
  return;
}


