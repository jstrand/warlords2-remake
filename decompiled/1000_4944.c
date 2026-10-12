
int __cdecl16far sprintf(char *buf,char *fmt,...)

{
  int iVar1;
  
  *buf = '\0';
  iVar1 = __vprinter(&stack0x000c,fmt,&buf);
  return iVar1;
}


