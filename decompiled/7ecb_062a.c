
undefined2 __cdecl16far FUN_7ecb_062a(char *param_1)

{
  char cVar1;
  bool bVar2;
  int fd;
  char *pcVar3;
  void *pvVar4;
  uint unaff_DI;
  long lVar5;
  char *local_c;
  undefined4 local_8;
  undefined2 local_4;
  
  bVar2 = false;
  fd = open(param_1,2);
  if (-1 < fd) {
    lVar5 = filelength(fd);
    pvVar4 = (void *)((ulong)lVar5 >> 0x10);
    local_4 = (undefined2)lVar5;
    pcVar3 = (char *)FUN_1d28_01d5(local_4);
    local_8 = (char *)CONCAT22(pvVar4,pcVar3);
    local_c = local_8;
    read(fd,(void *)CONCAT22(local_4,pvVar4),unaff_DI);
LAB_7ecb_06c0:
    if (!bVar2) {
      while( true ) {
        pcVar3 = (char *)local_c;
        if (*local_c == '#') break;
        local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
      }
      local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
      cVar1 = *local_c;
      local_c = (char *)CONCAT22(local_c._2_2_,pcVar3 + 2);
      if (cVar1 != 'D') goto code_r0x0007f354;
      FUN_7ecb_08e9(&local_c);
      goto LAB_7ecb_06bd;
    }
    FUN_1d28_050d((char *)local_8,(int)((ulong)local_8 >> 0x10));
    close(fd);
  }
  return 0;
code_r0x0007f354:
  if (cVar1 == 'E') {
LAB_7ecb_06bd:
    bVar2 = true;
  }
  goto LAB_7ecb_06c0;
}


