
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far armytype_lock(void)

{
  int fd;
  undefined2 uVar1;
  void *in_DX;
  uint unaff_SI;
  char *fmt;
  long lVar2;
  
  if (_LAB_4125_0e88 == 0) {
    uVar1 = DAT_3c04_0161;
    fmt = get_file_string(0x15,DAT_4125_26c4);
    sprintf(&DAT_451b_1cd8,fmt,uVar1);
    fd = open(&DAT_451b_1cd8,2);
    lVar2 = filelength(fd);
    in_DX = (void *)((ulong)lVar2 >> 0x10);
    uVar1 = mem_alloc((long)(int)lVar2,0,0xde,0);
    _LAB_4125_0e88 = CONCAT22(in_DX,uVar1);
    armytype_table._0_2_ = mem_lock(uVar1,in_DX);
    armytype_table._2_2_ = in_DX;
    read(fd,(void *)CONCAT22(0x706,in_DX),unaff_SI);
    close(fd);
    mem_unlock((int)_LAB_4125_0e88,(int)((ulong)_LAB_4125_0e88 >> 0x10));
  }
  armytype_table._0_2_ = mem_lock((int)_LAB_4125_0e88,(int)((ulong)_LAB_4125_0e88 >> 0x10));
  armytype_table._2_2_ = in_DX;
  return;
}


