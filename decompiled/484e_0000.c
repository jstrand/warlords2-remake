
void __cdecl16far FUN_484e_0000(void)

{
  if (opt_diplomacy != 0) {
    FUN_22bf_07a5();
    ui_diplomatic_ratings();
    DAT_4125_166a = 1;
    auto_str_join_data_not_found(0x15);
    FUN_484e_02ec();
    FUN_7ecb_0a1b();
    FUN_22bf_0757();
  }
  return;
}


