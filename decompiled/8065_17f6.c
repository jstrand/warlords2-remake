
void __cdecl16far next_side(void)

{
  bool bVar1;
  
  bVar1 = false;
  while (!bVar1) {
    if (DAT_3c04_0118 != 0) {
      DAT_3c04_0135 = DAT_3c04_0135 + 1;
      turn_order();
      DAT_3c04_0118 = 0;
      eliminate_sides();
      end_game_check();
      FUN_63f5_0034();
      neutral_production_turn();
      auto_file_current_hst();
      draw_turn_strip(1);
    }
    current_player = *(int *)(DAT_3c04_0173 * 2 + 0x163);
    DAT_3c04_0173 = DAT_3c04_0173 + 1;
    if (7 < DAT_3c04_0173) {
      DAT_3c04_0118 = 1;
    }
    if (*(int *)(current_player * 2 + 0x137) != 0) {
      bVar1 = true;
    }
  }
  return;
}


