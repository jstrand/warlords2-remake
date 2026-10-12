
uint __cdecl16far music_playing(void)

{
  uint uVar1;
  
  if (DAT_4125_37a3 == '\0') {
    return 0;
  }
  uVar1 = midpak_070c();
  if (uVar1 == 1) {
    return 1;
  }
  return uVar1 & 0xff00;
}


