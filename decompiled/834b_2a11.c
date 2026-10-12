
void __cdecl16far auto_file_mapcolor_dat(void)

{
  int fd;
  undefined2 uVar1;
  undefined2 uVar2;
  uint unaff_SI;
  char *fmt;
  
  uVar1 = DAT_3c04_0161;
  fmt = get_file_string(0x17,0);
  uVar2 = (undefined2)((ulong)fmt >> 0x10);
  sprintf(&DAT_451b_1cd8,fmt,uVar1);
  fd = open(&DAT_451b_1cd8,2);
  if (-1 < fd) {
    uVar1 = FUN_1d28_01d5(0x100);
    DAT_451b_1f32 = CONCAT22(uVar2,uVar1);
    uVar1 = FUN_1d28_01d5(0x100);
    DAT_451b_1f2e = CONCAT22(uVar2,uVar1);
    uVar1 = FUN_1d28_01d5(0x100);
    DAT_451b_1f2a = CONCAT22(uVar2,uVar1);
    uVar1 = FUN_1d28_01d5(0x100);
    DAT_451b_1f26 = CONCAT22(uVar2,uVar1);
    uVar1 = FUN_1d28_01d5(0x14);
    DAT_451b_1f22 = CONCAT22(uVar2,uVar1);
    uVar1 = FUN_1d28_01d5(0x14);
    DAT_451b_1f1e = CONCAT22(uVar2,uVar1);
    uVar1 = FUN_1d28_01d5(0x14);
    DAT_451b_1f1a = CONCAT22(uVar2,uVar1);
    uVar1 = FUN_1d28_01d5(0x14);
    DAT_451b_1f16 = CONCAT22(uVar2,uVar1);
    uVar1 = FUN_1d28_01d5(0x20);
    DAT_451b_1f12 = CONCAT22(uVar2,uVar1);
    uVar1 = FUN_1d28_01d5(0x20);
    DAT_451b_1f0e = CONCAT22(uVar2,uVar1);
    uVar1 = FUN_1d28_01d5(0x20);
    DAT_451b_1f0a = CONCAT22(uVar2,uVar1);
    read(fd,(void *)CONCAT22(0x100,(void *)((ulong)DAT_451b_1f32 >> 0x10)),unaff_SI);
    read(fd,(void *)CONCAT22(0x100,(void *)((ulong)DAT_451b_1f2e >> 0x10)),unaff_SI);
    read(fd,(void *)CONCAT22(0x100,(void *)((ulong)DAT_451b_1f2a >> 0x10)),unaff_SI);
    read(fd,(void *)CONCAT22(0x100,(void *)((ulong)DAT_451b_1f26 >> 0x10)),unaff_SI);
    read(fd,(void *)CONCAT22(0x20,(void *)((ulong)DAT_451b_1f12 >> 0x10)),unaff_SI);
    read(fd,(void *)CONCAT22(0x20,(void *)((ulong)DAT_451b_1f0e >> 0x10)),unaff_SI);
    read(fd,(void *)CONCAT22(0x20,(void *)((ulong)DAT_451b_1f0a >> 0x10)),unaff_SI);
    read(fd,(void *)CONCAT22(0x14,(void *)((ulong)DAT_451b_1f22 >> 0x10)),unaff_SI);
    read(fd,(void *)CONCAT22(0x14,(void *)((ulong)DAT_451b_1f1e >> 0x10)),unaff_SI);
    read(fd,(void *)CONCAT22(0x14,(void *)((ulong)DAT_451b_1f1a >> 0x10)),unaff_SI);
    read(fd,(void *)CONCAT22(0x14,(void *)((ulong)DAT_451b_1f16 >> 0x10)),unaff_SI);
    close(fd);
  }
  return;
}


