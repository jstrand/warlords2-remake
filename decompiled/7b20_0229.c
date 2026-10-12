
void __cdecl16far
FUN_7b20_0229(char *param_1,int param_2,undefined2 param_3,int param_4,undefined2 param_5,
             int param_6,undefined2 param_7)

{
  int fd;
  int iVar1;
  uint unaff_DI;
  void *unaff_SS;
  char local_12 [10];
  undefined1 local_8;
  
  fd = open(param_1,2);
  if (-1 < fd) {
    for (iVar1 = 0; iVar1 < 0x10; iVar1 = iVar1 + 1) {
      read(fd,(void *)CONCAT22(10,unaff_SS),unaff_DI);
      local_8 = 0;
      sscanf((char *)CONCAT22(unaff_SS,local_12),(char *)0x41251f4a,param_2 + iVar1 * 2,param_3,
             param_4 + iVar1 * 2,param_5,param_6 + iVar1 * 2,param_7);
    }
    close(fd);
  }
  return;
}


