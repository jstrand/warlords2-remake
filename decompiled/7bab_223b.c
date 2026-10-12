
void __cdecl16far auto_file_options_dat(void)

{
  int iVar1;
  int iVar2;
  uint unaff_DI;
  char *name;
  
  iVar1 = 2;
  name = get_file_string(0x43,0);
  iVar1 = open(name,iVar1);
  if (-1 < iVar1) {
    for (iVar2 = 0; iVar2 < 10; iVar2 = iVar2 + 1) {
      read(iVar1,(void *)0x24125,unaff_DI);
    }
    close(iVar1);
  }
  return;
}


