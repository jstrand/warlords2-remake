
long __cdecl16far dos_tell(int handle)

{
  code *pcVar1;
  long lVar2;
  
  pcVar1 = (code *)swi(0x21);
  lVar2 = (*pcVar1)();
  return lVar2;
}


