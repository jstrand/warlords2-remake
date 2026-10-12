
void __cdecl16far auto_tutorial_hook_4(void)

{
  char *pcVar1;
  
  FUN_54f6_069c();
  FUN_8065_011f(1,0x10001);
  if ((opt_tutorial != 0) && ((DAT_3c04_0134 & 4) == 0)) {
    DAT_3c04_0134 = DAT_3c04_0134 | 4;
    pcVar1 = get_file_string(0x19,2);
    FUN_8065_168d((char *)pcVar1,(int)((ulong)pcVar1 >> 0x10));
  }
  return;
}


