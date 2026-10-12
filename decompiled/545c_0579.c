
undefined2 __cdecl16far auto_file_udb_dat_3(int param_1,int *param_2)

{
  int fd;
  uint unaff_DI;
  int iVar1;
  void *unaff_SS;
  undefined2 local_4a;
  undefined2 local_6;
  undefined2 local_4;
  
  local_6 = 0;
  *param_2 = -1;
  fd = open((char *)0x412505ba,2);
  if (fd < 0) {
    local_6 = 0;
  }
  else {
    read(fd,(void *)CONCAT22(2,unaff_SS),unaff_DI);
    for (iVar1 = 0; iVar1 < local_4; iVar1 = iVar1 + 1) {
      read(fd,(void *)CONCAT22(0x44,unaff_SS),unaff_DI);
      if (local_4a == param_1) {
        local_6 = 1;
        *param_2 = iVar1;
        break;
      }
    }
    close(fd);
  }
  return local_6;
}


