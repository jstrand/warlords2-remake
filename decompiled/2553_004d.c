
bool __cdecl16far FUN_2553_004d(void)

{
  code *pcVar1;
  undefined1 in_CF;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return !(bool)in_CF;
}


