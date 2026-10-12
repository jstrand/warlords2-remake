
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far save_game(char *param_1,int param_2,char *param_3)

{
  int fd;
  int iVar1;
  undefined2 uVar2;
  void *pvVar3;
  uint unaff_DI;
  void *unaff_SS;
  char *pcVar4;
  char local_5a [80];
  undefined2 local_a;
  void *pvStack_8;
  long local_6;
  
  if (param_1 != (char *)0x0) {
    strcpy((char *)CONCAT22(0x4125,(char *)(_LAB_4125_128b_1 * 0xf + 0x471c)),param_1);
    ((undefined1 *)&DAT_451b_07b2)[_LAB_4125_128b_1] = 1;
    auto_file_saveinfo_dat_2();
  }
  if (param_2 == 0) {
    pcVar4 = get_file_string(0x1c,_LAB_4125_128b_1);
    pvVar3 = (void *)((ulong)pcVar4 >> 0x10);
    sprintf((char *)CONCAT22(unaff_SS,local_5a),pcVar4);
  }
  else {
    pcVar4 = strcpy((char *)CONCAT22(unaff_SS,local_5a),param_3);
    pvVar3 = (void *)((ulong)pcVar4 >> 0x10);
  }
  FUN_22bf_07a5();
  fd = open((char *)CONCAT22(unaff_SS,local_5a),9);
  if (-1 < fd) {
    write(fd,(void *)0x544125,unaff_DI);
    write(fd,(void *)0x2ee13c04,unaff_DI);
    write(fd,(void *)0xde702e1d,unaff_DI);
    write(fd,(void *)0x444029d9,unaff_DI);
    _LAB_4125_126b_1 = mem_lock((int)_LAB_4125_126f_1,(int)((ulong)_LAB_4125_126f_1 >> 0x10));
    _LAB_4125_126d_1 = pvVar3;
    write(fd,(void *)CONCAT22((undefined2 *)&DAT_451b_04e0,pvVar3),unaff_DI);
    mem_unlock((int)_LAB_4125_126f_1,(int)((ulong)_LAB_4125_126f_1 >> 0x10));
    iVar1 = 2;
    pcVar4 = get_file_string(0x1d,0);
    iVar1 = open(pcVar4,iVar1);
    local_6 = filelength(iVar1);
    pvVar3 = (void *)((ulong)local_6 >> 0x10);
    local_a = FUN_1d28_01d5((int)local_6);
    pvStack_8 = pvVar3;
    read(iVar1,(void *)CONCAT22((undefined2)local_6,pvVar3),unaff_DI);
    write(fd,(void *)CONCAT22(4,unaff_SS),unaff_DI);
    write(fd,(void *)CONCAT22((undefined2)local_6,pvStack_8),unaff_DI);
    close(iVar1);
    FUN_1d28_050d(local_a,pvStack_8);
    if (DAT_3c04_0120 != 0) {
      iVar1 = 2;
      pcVar4 = get_file_string(0x22,0);
      iVar1 = open(pcVar4,iVar1);
      local_6 = filelength(iVar1);
      pvVar3 = (void *)((ulong)local_6 >> 0x10);
      local_a = FUN_1d28_01d5((int)local_6);
      pvStack_8 = pvVar3;
      read(iVar1,(void *)CONCAT22((undefined2)local_6,pvVar3),unaff_DI);
      write(fd,(void *)CONCAT22(4,unaff_SS),unaff_DI);
      write(fd,(void *)CONCAT22((undefined2)local_6,pvStack_8),unaff_DI);
      close(iVar1);
      FUN_1d28_050d(local_a,pvStack_8);
    }
    if (DAT_3c04_0120 != 0) {
      iVar1 = 2;
      pcVar4 = get_file_string(0x23,0);
      iVar1 = open(pcVar4,iVar1);
      local_6 = filelength(iVar1);
      pvVar3 = (void *)((ulong)local_6 >> 0x10);
      local_a = FUN_1d28_01d5((int)local_6);
      pvStack_8 = pvVar3;
      read(iVar1,(void *)CONCAT22((undefined2)local_6,pvVar3),unaff_DI);
      write(fd,(void *)CONCAT22(4,unaff_SS),unaff_DI);
      write(fd,(void *)CONCAT22((undefined2)local_6,pvStack_8),unaff_DI);
      close(iVar1);
      FUN_1d28_050d(local_a,pvStack_8);
    }
    iVar1 = 2;
    pcVar4 = get_file_string(6,0);
    iVar1 = open(pcVar4,iVar1);
    local_6 = filelength(iVar1);
    pvVar3 = (void *)((ulong)local_6 >> 0x10);
    uVar2 = FUN_1d28_01d5((int)local_6);
    _local_a = CONCAT22(pvVar3,uVar2);
    read(iVar1,(void *)CONCAT22((undefined2)local_6,pvVar3),unaff_DI);
    write(fd,(void *)CONCAT22(4,unaff_SS),unaff_DI);
    write(fd,(void *)CONCAT22((undefined2)local_6,(void *)((ulong)_local_a >> 0x10)),unaff_DI);
    close(iVar1);
    FUN_1d28_050d((int)_local_a,(int)((ulong)_local_a >> 0x10));
    if (param_2 == 0) {
      save_ai_data(fd);
    }
    close(fd);
  }
  FUN_22bf_0757();
  return;
}


