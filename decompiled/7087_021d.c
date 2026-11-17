
void __cdecl16far FUN_7087_021d(void)

{
  *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + DAT_4125_0e98 * 0x41) = 1;
  *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + DAT_4125_0e98 * 0x41) = 0xfffe;
  *(undefined2 *)(DAT_4125_0e98 * 0x41 + 0x15b2) = 0xffff;
  FUN_22bf_07a5();
  FUN_54f6_07e4(0x280000,0x320000);
  auto_ui_city_info();
  city_dialog_draw_map();
  city_dialog_buttons();
  FUN_22bf_0757();
  return;
}


