
void __cdecl16far auto_file_udb_cur_2(void)

{
  int fd;
  uint unaff_SI;
  
  fd = open((char *)0x41250596,1);
  if (-1 < fd) {
    write(fd,(void *)0x84125,unaff_SI);
    close(fd);
  }
  return;
}


