
void __cdecl16far bios_puts(char *s)

{
  char *pcVar1;
  code *pcVar2;
  char *pcVar3;
  undefined2 uVar4;
  
  uVar4 = 0x4125;
  pcVar3 = (char *)s;
  while( true ) {
    pcVar1 = pcVar3;
    pcVar3 = pcVar3 + 1;
    if (*pcVar1 == '\0') break;
    if (*pcVar1 == '\n') {
      pcVar2 = (code *)swi(0x10);
      (*pcVar2)(uVar4);
    }
    pcVar2 = (code *)swi(0x10);
    (*pcVar2)();
  }
  return;
}


