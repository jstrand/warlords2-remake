
void __cdecl16far digpak_play2(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x66);
  (*pcVar1)(0x4125);
  return;
}


