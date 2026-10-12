
int __cdecl16far putchar(int __c)

{
  int iVar1;
  
  iVar1 = fputc(__c,(FILE *)0x3b80);
  return iVar1;
}


