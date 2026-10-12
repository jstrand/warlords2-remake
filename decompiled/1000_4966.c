
int __cdecl16far vsprintf(char *__s,char *__format,__gnuc_va_list __arg)

{
  int iVar1;
  
  *___s = '\0';
  iVar1 = __vprinter();
  return iVar1;
}


