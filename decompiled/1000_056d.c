
long __cdecl16far dos_seek(int handle,long pos)

{
  code *pcVar1;
  undefined1 in_CF;
  long lVar2;
  
  pcVar1 = (code *)swi(0x21);
  lVar2 = (*pcVar1)();
  if (!(bool)in_CF) {
    return lVar2;
  }
  return 0;
}


