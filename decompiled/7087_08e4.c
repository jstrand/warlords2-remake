
void __cdecl16far ui_city_dialog_map(void)

{
  if (LAB_4125_0e92 != 2) {
    if (LAB_4125_0e92 != 3) {
      return;
    }
    if (DAT_4125_0e96 != 0) {
      draw_vector_map_all((int *)0x0,DAT_4125_0e98);
      goto LAB_7087_0907;
    }
  }
  draw_vector_map((int *)0x0,DAT_4125_0e98,-2);
LAB_7087_0907:
  FUN_834b_04d3(0,0);
  return;
}


