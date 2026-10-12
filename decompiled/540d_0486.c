
void __cdecl16far auto_file_current_sgn_3(void)

{
  int fd;
  uint unaff_SI;
  
  fd = open(s_current_sgn_4125_041f,9);
  if (-1 < fd) {
    write(fd,(void *)CONCAT22(DAT_451b_003c,(void *)((ulong)DAT_451b_003e >> 0x10)),unaff_SI);
    close(fd);
    FUN_1d28_050d((int)DAT_451b_003e,(int)((ulong)DAT_451b_003e >> 0x10));
    FUN_54f6_069c();
    game_screen_start(0,0);
  }
  return;
}


