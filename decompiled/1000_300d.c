
char __cdecl16near __video_int(void)

{
  code *pcVar1;
  char cVar2;
  char cVar3;
  undefined2 in_AX;
  
  cVar2 = (char)((uint)in_AX >> 8);
  if (cVar2 == '\0') {
    cVar2 = (char)in_AX;
    if ((cVar2 == '\x02') || (cVar2 == '\x03')) {
      pcVar1 = (code *)swi(0x10);
      cVar2 = (*pcVar1)(0x4125);
      if (cVar2 != '\x1a') {
        DAT_0000_0487 = DAT_0000_0487 & 0xfe;
      }
      pcVar1 = (code *)swi(0x10);
      (*pcVar1)();
    }
    else if (cVar2 == '@') {
      cVar3 = '\x10';
      pcVar1 = (code *)swi(0x10);
      cVar2 = (*pcVar1)();
      if (cVar3 == '\x10') {
        return cVar2;
      }
      pcVar1 = (code *)swi(0x10);
      (*pcVar1)();
      pcVar1 = (code *)swi(0x10);
      (*pcVar1)();
      pcVar1 = (code *)swi(0x10);
      cVar2 = (*pcVar1)();
      if (cVar2 == '\x1a') {
        return '\x1a';
      }
      DAT_0000_0487 = DAT_0000_0487 | 1;
      pcVar1 = (code *)swi(0x10);
      cVar2 = (*pcVar1)();
      return cVar2;
    }
  }
  else if (cVar2 == '\x0f') {
    pcVar1 = (code *)swi(0x10);
    cVar2 = (*pcVar1)();
    if ((cVar2 != '\x02') && (cVar2 != '\x03')) {
      return cVar2;
    }
    cVar3 = __ega_installed();
    if (cVar3 == '\0') {
      return cVar2;
    }
    if (DAT_0000_0484 == '\x18') {
      return cVar2;
    }
    return '@';
  }
  pcVar1 = (code *)swi(0x10);
  cVar2 = (*pcVar1)();
  return cVar2;
}


