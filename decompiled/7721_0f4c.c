
void __cdecl16far auto_file_saveinfo_dat_2(void)

{
  uint uVar1;
  int iVar2;
  uint unaff_DI;
  int iVar3;
  void *unaff_SS;
  char *name;
  char *pcVar4;
  char local_7c [50];
  char local_4a [50];
  char local_18 [20];
  int local_4;
  
  iVar3 = 1;
  name = get_file_string(4,0);
  local_4 = open(name,iVar3);
  for (iVar3 = 0; iVar3 < 10; iVar3 = iVar3 + 1) {
    strcpy((char *)CONCAT22(unaff_SS,local_18),
           (char *)CONCAT22(0x4125,(char *)(iVar3 * 0xf + 0x471c)));
    iVar2 = 0;
    while( true ) {
      uVar1 = strlen((char *)CONCAT22(unaff_SS,local_18));
      if ((int)uVar1 <= iVar2) break;
      if (local_18[iVar2] == ' ') {
        local_18[iVar2] = '_';
      }
      iVar2 = iVar2 + 1;
    }
    uVar1 = strlen((char *)CONCAT22(unaff_SS,local_18));
    sprintf((char *)CONCAT22(unaff_SS,local_7c),(char *)0x412512a3,uVar1);
    pcVar4 = local_18;
    uVar1 = strlen((char *)CONCAT22(unaff_SS,pcVar4));
    sprintf((char *)CONCAT22(unaff_SS,local_4a),(char *)CONCAT22(unaff_SS,local_7c),
            (int)(char)((undefined1 *)&DAT_451b_07b2)[iVar3],uVar1 + 1,pcVar4);
    uVar1 = strlen((char *)CONCAT22(unaff_SS,local_4a));
    write(local_4,(void *)CONCAT22(uVar1,unaff_SS),unaff_DI);
  }
  close(local_4);
  return;
}


