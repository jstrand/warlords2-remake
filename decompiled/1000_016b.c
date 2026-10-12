
bool __cdecl16far _terminate(void)

{
  code *pcVar1;
  char cVar2;
  int iVar3;
  int in_BX;
  int unaff_SI;
  undefined2 uVar4;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  uVar4 = uRam00010291;
  __error_display();
  cVar2 = '\x03';
  _exit(3);
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + cVar2;
  iVar3 = sound_driver_present();
  if (iVar3 != 0) {
    pcVar1 = (code *)swi(0x66);
    (*pcVar1)(uVar4);
  }
  return iVar3 != 0;
}


