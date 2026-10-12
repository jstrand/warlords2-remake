
void __cdecl16far auto_file_rmaptemp_dat(void)

{
  char *pcVar1;
  
  pcVar1 = get_file_string(0x1b,0);
  save_game(0,0x10000,(char *)pcVar1,(int)((ulong)pcVar1 >> 0x10));
  return;
}


