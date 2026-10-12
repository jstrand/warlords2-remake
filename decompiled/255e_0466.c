
void __cdecl16far music_stop(void)

{
  if (DAT_4125_37a3 != '\0') {
    midpak_stop();
    if ((DAT_4125_37a9 & 2) != 0) {
      music_is_loaded();
    }
    DAT_4125_379e = 0xffff;
    DAT_4125_37a9 = 0;
  }
  return;
}


