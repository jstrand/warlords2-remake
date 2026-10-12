
void __cdecl16far random_map_highlands(void)

{
  *(undefined2 *)((int)DAT_451b_002a + 0x611e) = 0;
  FUN_4d71_0495();
  FUN_4d71_03af();
  random_map_link_nodes();
  random_map_draw_ridges();
  random_map_smooth_highlands();
  FUN_4d71_033a();
  return;
}


