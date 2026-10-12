
void __cdecl16far sound_poll(void)

{
  char cVar1;
  
  if (DAT_4125_379e != -1) {
    cVar1 = music_playing();
    if (cVar1 == '\0') {
      if ((DAT_4125_37a9 & 1) == 0) {
        if ((DAT_4125_37a9 & 2) == 0) {
          music_stop();
        }
        else {
          music_stop();
        }
      }
      else {
        music_play(DAT_4125_379e,DAT_4125_37a9);
      }
    }
  }
  if (DAT_4125_37a0 != -1) {
    cVar1 = sample_playing();
    if (cVar1 == '\0') {
      if ((DAT_4125_37a7 & 1) != 0) {
        sample_play(DAT_4125_37a0,DAT_4125_37a7);
        return;
      }
      if ((DAT_4125_37a7 & 2) != 0) {
        sample_finish();
        DAT_4125_37a0 = 0xffff;
        return;
      }
      DAT_4125_37a0 = -1;
    }
  }
  return;
}


