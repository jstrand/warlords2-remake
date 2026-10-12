
char * __cdecl16far itoa(int value,char *buf,int radix)

{
  int iVar1;
  char *pcVar2;
  
  if (radix == 10) {
    iVar1 = value >> 0xf;
  }
  else {
    iVar1 = 0;
  }
  pcVar2 = (char *)__longtoa(0x61,1,radix,buf,value,iVar1);
  return pcVar2;
}


