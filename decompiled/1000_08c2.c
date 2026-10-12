
int __cdecl16far dos_delete(char *name)

{
  code *pcVar1;
  undefined1 in_CF;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return (uint)!(bool)in_CF;
}


