
bool __cdecl16far FUN_7ecb_0375(char *param_1)

{
  int fd;
  
  fd = open(param_1,2);
  if (-1 < fd) {
    close(fd);
  }
  return -1 < fd;
}


