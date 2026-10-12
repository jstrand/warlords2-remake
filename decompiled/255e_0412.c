
void __cdecl16far music_play(undefined2 param_1,undefined2 param_2)

{
  char cVar1;
  int iVar2;
  
  if (DAT_4125_37a3 != '\0') {
    cVar1 = music_playing();
    if (cVar1 != '\0') {
      music_stop();
    }
    DAT_4125_379e = param_1;
    iVar2 = midpak_register_xmidi(DAT_451b_30f0,DAT_451b_30f2,DAT_451b_30f8,DAT_451b_30fa);
    if (iVar2 != 0) {
      midpak_play_sequence(param_1);
      DAT_4125_37a9 = param_2;
    }
  }
  return;
}


