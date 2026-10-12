
undefined2 __cdecl16far auto_file_terrain_dat_2(int param_1,undefined4 param_2)

{
  int iVar1;
  uint unaff_DI;
  int iVar2;
  void *unaff_SS;
  char *name;
  
  iVar1 = 2;
  name = get_file_string(0x24,0);
  iVar1 = open(name,iVar1);
  if (iVar1 != 0) {
    read(iVar1,(void *)CONCAT22(2,unaff_SS),unaff_DI);
    if (param_1 < 1) {
      for (iVar2 = 0; iVar2 <= param_1; iVar2 = iVar2 + 1) {
        read(iVar1,(void *)CONCAT22(0x34,(void *)((ulong)param_2 >> 0x10)),unaff_DI);
      }
      close(iVar1);
      return 1;
    }
    close(iVar1);
  }
  return 0;
}


