
void __cdecl16far city_dialog_draw_map(void)

{
  if (LAB_4125_0e92 < 5) {
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)(LAB_4125_0e92 * 2 + 0x1af0)) {
    case 0x73aee:
      FUN_834b_0000(1,1,0,0);
      FUN_834b_0ed7(0,0,DAT_4125_0e98);
      break;
    case 0x73b1b:
      FUN_834b_05e4(0,0,DAT_4125_0e98);
      break;
    case 0x73b29:
      ui_city_dialog_map();
      return;
    }
    FUN_834b_04d3(0,0);
  }
  return;
}


