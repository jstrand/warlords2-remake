
void __cdecl16far midpak_unload_driver(void)

{
  if (DAT_1000_0d31 != 0) {
    (*DAT_4125_0d37)(0x1000);
    FUN_255e_07ea(DAT_1000_0d2d,DAT_1000_0d2f);
    DAT_1000_0d31 = 0;
  }
  return;
}


