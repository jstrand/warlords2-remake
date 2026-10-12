
int __cdecl16far sample_playing(void)

{
  int iVar1;
  
  if (DAT_4125_37a2 == '\0') {
    return 0;
  }
  if (DAT_4125_37ab != '\0') {
    return (uint)(byte)(DAT_4125_37a2 >> 7) << 8;
  }
  iVar1 = digpak_sound_status();
  return iVar1;
}


