
void __cdecl16far auto_file_saveinfo_dat(void)

{
  uint uVar1;
  int iVar2;
  uint unaff_DI;
  int iVar3;
  void *unaff_SS;
  char *name;
  char local_1e [20];
  int local_a;
  char local_7;
  int local_6;
  uint local_4;
  
  iVar3 = 2;
  name = get_file_string(4,0);
  local_a = open(name,iVar3);
  for (iVar3 = 0; iVar3 < 10; iVar3 = iVar3 + 1) {
    read(local_a,(void *)CONCAT22(1,unaff_SS),unaff_DI);
    local_4 = (uint)(local_7 == '1');
    read(local_a,(void *)CONCAT22(2,unaff_SS),unaff_DI);
    local_1e[2] = 0;
    sscanf((char *)CONCAT22(unaff_SS,local_1e),(char *)0x412512a0,&local_6);
    read(local_a,(void *)CONCAT22(local_6 + 1,unaff_SS),unaff_DI);
    (&stack0xffe1)[local_6] = 0;
    ((undefined1 *)&DAT_451b_07b2)[iVar3] = (undefined1)local_4;
    for (iVar2 = 0; uVar1 = strlen((char *)CONCAT22(unaff_SS,local_1e)), iVar2 < (int)uVar1;
        iVar2 = iVar2 + 1) {
      if (local_1e[iVar2] == '_') {
        local_1e[iVar2] = ' ';
      }
    }
    strcpy((char *)CONCAT22(0x4125,(char *)(iVar3 * 0xf + 0x471c)),
           (char *)CONCAT22(unaff_SS,local_1e));
  }
  close(local_a);
  return;
}


