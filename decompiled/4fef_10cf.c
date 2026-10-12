
void __cdecl16far auto_file_random_scn(void)

{
  int fd;
  uint unaff_SI;
  undefined2 unaff_SS;
  char *pcVar1;
  char *pcVar2;
  char local_2a [40];
  
  pcVar1 = get_file_string(0x20,0);
  sprintf((char *)CONCAT22(unaff_SS,local_2a),pcVar1);
  fd = open((char *)CONCAT22(unaff_SS,local_2a),9);
  if (fd < 0) {
    pcVar2 = local_2a;
    pcVar1 = get_error_string(0,0);
    printf(pcVar1,pcVar2);
  }
  else {
    write(fd,(void *)0x2ee13c04,unaff_SI);
    close(fd);
  }
  return;
}


