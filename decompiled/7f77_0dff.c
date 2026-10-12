
undefined2 __cdecl16far auto_file_terrain_dat(void)

{
  int iVar1;
  uint unaff_SI;
  void *unaff_SS;
  char *name;
  
  iVar1 = 2;
  name = get_file_string(0x24,0);
  iVar1 = open(name,iVar1);
  if (iVar1 != 0) {
    read(iVar1,(void *)CONCAT22(2,unaff_SS),unaff_SI);
    close(iVar1);
  }
  return 1;
}


