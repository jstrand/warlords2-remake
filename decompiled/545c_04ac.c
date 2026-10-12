
undefined2 __cdecl16far auto_file_udb_dat(int param_1,undefined4 param_2)

{
  int fd;
  uint unaff_DI;
  int iVar1;
  void *unaff_SS;
  int local_4;
  
  fd = open((char *)0x412505a2,2);
  if ((-1 < fd) && (read(fd,(void *)CONCAT22(2,unaff_SS),unaff_DI), param_1 < local_4)) {
    for (iVar1 = 0; iVar1 <= param_1; iVar1 = iVar1 + 1) {
      read(fd,(void *)CONCAT22(0x44,(void *)((ulong)param_2 >> 0x10)),unaff_DI);
    }
    close(fd);
    return 1;
  }
  return 0;
}


