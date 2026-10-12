
void __cdecl16far FUN_7721_0d61(char *param_1,char *param_2)

{
  int fd;
  int fd_00;
  undefined2 uVar1;
  long lVar2;
  void *pvVar3;
  uint unaff_DI;
  undefined2 local_a;
  undefined4 local_6;
  
  fd = open(param_1,2);
  if (-1 < fd) {
    fd_00 = open(param_2,9);
    if (fd_00 < 0) {
      close(fd);
    }
    else {
      local_6 = filelength(fd);
      pvVar3 = (void *)((ulong)local_6 >> 0x10);
      uVar1 = FUN_1d28_01d5(30000);
      for (; 0 < local_6; local_6 = local_6 - lVar2) {
        read(fd,(void *)CONCAT22(30000,pvVar3),unaff_DI);
        lVar2 = local_6;
        if (29999 < local_6) {
          lVar2 = 30000;
        }
        local_a = (undefined2)lVar2;
        write(fd_00,(void *)CONCAT22(local_a,pvVar3),unaff_DI);
      }
      close(fd);
      close(fd_00);
      FUN_1d28_050d(uVar1,pvVar3);
    }
  }
  return;
}


