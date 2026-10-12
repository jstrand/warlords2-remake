
void __cdecl16far music_disable(void)

{
  char cVar1;
  
  if (DAT_4125_37a6 == '\0') {
    cVar1 = music_playing();
    if (cVar1 != '\0') {
      music_stop();
    }
    DAT_4125_37a3 = 0;
  }
  return;
}


