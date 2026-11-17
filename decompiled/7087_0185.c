
void __cdecl16far ui_stop_production(void)

{
  *(undefined1 *)(DAT_4125_0e98 * 0x41 + 0x15aa) = 0;
  *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + DAT_4125_0e98 * 0x41) = 0;
  *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + DAT_4125_0e98 * 0x41) = 0xffff;
  *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x15b2) = 0xffff;
  DAT_4125_0e9e = 0xffff;
  *(undefined1 *)(DAT_4125_0e98 * 0x41 + 0x15a9) = 0xff;
  FUN_22bf_07a5();
  FUN_54f6_07e4(0x280000,0x320000);
  auto_ui_city_info();
  city_dialog_draw_map();
  city_dialog_buttons();
  FUN_22bf_0757();
  return;
}


