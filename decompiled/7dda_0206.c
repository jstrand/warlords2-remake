
void __cdecl16far sound_war(void)

{
  int iVar1;
  char *pcVar2;
  
  if (DAT_4125_0bec != 0) {
    pcVar2 = get_file_string(0x26,0);
    iVar1 = sample_load((char *)pcVar2,(int)((ulong)pcVar2 >> 0x10));
    if (-1 < iVar1) {
      sample_play(iVar1,2);
      sample_wait();
    }
  }
  return;
}


