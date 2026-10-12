
void __cdecl16far FUN_7715_006a(undefined2 param_1,undefined4 param_2)

{
  int fd;
  uint unaff_SI;
  
  FUN_7bab_21c1(param_1,1,(undefined1 *)&DAT_451b_1cd8,0x4125);
  fd = open(&DAT_451b_1cd8,2);
  if (-1 < fd) {
    read(fd,(void *)CONCAT22(0x67,(void *)((ulong)param_2 >> 0x10)),unaff_SI);
    close(fd);
  }
  return;
}


