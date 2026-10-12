
int __cdecl16far get_cwd(char *buf)

{
  char *pcVar1;
  code *pcVar2;
  char cVar3;
  uint uVar4;
  char *unaff_ES;
  undefined2 uVar5;
  undefined2 uVar6;
  
  uVar6 = 0x4125;
  uVar5 = (undefined2)((ulong)buf >> 0x10);
  pcVar2 = (code *)swi(0x21);
  cVar3 = (*pcVar2)();
  *buf = cVar3 + 'A';
  ((char *)buf)[1] = ':';
  ((char *)buf)[2] = '\\';
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)(uVar6);
  uVar4 = 0xffff;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    pcVar1 = unaff_ES;
    unaff_ES = unaff_ES + 1;
  } while (*pcVar1 != '\0');
  return ~uVar4 - 1;
}


