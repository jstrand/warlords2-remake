
void __cdecl16far sample_wait(void)

{
  int iVar1;
  
  if (((DAT_4125_37a2 != '\0') && (DAT_4125_37ab == '\0')) && (DAT_4125_37a0 != -1)) {
    do {
      iVar1 = digpak_sound_status();
    } while (iVar1 != 0);
    sample_finish();
  }
  return;
}


