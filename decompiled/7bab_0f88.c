
void __cdecl16far auto_file_quotes_dat(void)

{
  int iVar1;
  int iVar2;
  uint unaff_DI;
  int iVar3;
  void *unaff_SS;
  char *name;
  char local_108 [256];
  uint local_8;
  int local_6;
  int local_4;
  
  iVar1 = 2;
  name = get_file_string(2,0);
  iVar1 = open(name,iVar1);
  if (-1 < iVar1) {
    read(iVar1,(void *)CONCAT22(4,unaff_SS),unaff_DI);
    local_108[2] = 0;
    sscanf((char *)CONCAT22(unaff_SS,local_108),(char *)0x41252595,&local_4);
    local_6 = dice(1,local_4,0);
    for (iVar2 = 0; iVar2 < local_6; iVar2 = iVar2 + 1) {
      read(iVar1,(void *)CONCAT22(3,unaff_SS),unaff_DI);
      local_108[3] = 0;
      sscanf((char *)CONCAT22(unaff_SS,local_108),(char *)0x41252599,&local_4);
      read(iVar1,(void *)CONCAT22(local_4 + 1,unaff_SS),unaff_DI);
      local_108[local_4 + -1] = '\0';
    }
    close(iVar1);
    FUN_78a8_06ae(2,0xf,0,3);
    FUN_22bf_07a5();
    local_8 = strlen((char *)CONCAT22(unaff_SS,local_108));
    iVar1 = 0;
    iVar3 = 0;
    for (iVar2 = 0; iVar2 < (int)local_8; iVar2 = iVar2 + 1) {
      if (local_108[iVar2] == '|') {
        local_108[iVar2] = '\0';
        FUN_7ecb_00d6(((undefined2 *)&DAT_4125_253c)[iVar3 * 2],
                      ((undefined2 *)&DAT_4125_253e)[iVar3 * 2],local_108 + iVar1);
        iVar3 = iVar3 + 1;
        iVar1 = iVar2 + 1;
      }
    }
    FUN_22bf_0757();
  }
  return;
}


