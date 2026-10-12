
void __cdecl16far random_map_generate(void)

{
  random_map_progress(0);
  auto_file_x_scn();
  FUN_4bed_036b();
  FUN_4c49_0000();
  random_map_progress(10);
  random_map_highlands();
  random_map_progress(0x14);
  FUN_4f5f_0000();
  random_map_progress(0x1e);
  FUN_4eb7_0000();
  random_map_progress(0x28);
  random_map_forests();
  random_map_progress(0x32);
  random_map_marshes();
  random_map_progress(0x3c);
  FUN_513d_0000();
  random_map_progress(0x46);
  auto_str_03d_is_inhabited_by();
  random_map_progress(0x50);
  FUN_5311_0000();
  random_map_progress(0x5a);
  FUN_4fef_0014();
  random_map_progress(100);
  return;
}


