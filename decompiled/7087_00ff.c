
void __cdecl16far city_start_chosen_production(void)

{
  int iVar1;
  
  iVar1 = city_slot_of_type(DAT_4125_0e98,DAT_4125_0e9e);
  *(undefined1 *)(DAT_4125_0e98 * 0x41 + 0x15a9) = (undefined1)DAT_4125_0e9e;
  *(undefined1 *)(DAT_4125_0e98 * 0x41 + 0x15aa) =
       *(undefined1 *)(DAT_4125_0e98 * 0x41 + iVar1 + 0x1597);
  FUN_22bf_07a5();
  FUN_54f6_07e4(0x280000,0x320000);
  auto_ui_city_info();
  city_dialog_draw_map();
  city_dialog_buttons();
  FUN_22bf_0757();
  return;
}


