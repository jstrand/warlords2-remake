
void __cdecl16far load_string_files(void)

{
  int iVar1;
  undefined2 uVar2;
  void *pvVar3;
  uint unaff_DI;
  long lVar4;
  
  iVar1 = open(s_FDATA_STRING_DAT_4125_2693 + 1,2);
  if (-1 < iVar1) {
    lVar4 = filelength(iVar1);
    pvVar3 = (void *)((ulong)lVar4 >> 0x10);
    uVar2 = mem_alloc((long)(int)lVar4,0,0xa6,0);
    h_string_dat = CONCAT22(pvVar3,uVar2);
    mem_lock(uVar2,pvVar3);
    read(iVar1,(void *)CONCAT22((int)lVar4,pvVar3),unaff_DI);
    mem_unlock((int)h_string_dat,(int)((ulong)h_string_dat >> 0x10));
    close(iVar1);
    iVar1 = open(s_DATA_ERROR_DAT_4125_26a4,2);
    if (-1 < iVar1) {
      lVar4 = filelength(iVar1);
      pvVar3 = (void *)((ulong)lVar4 >> 0x10);
      uVar2 = mem_alloc((long)(int)lVar4,0,0xa7,0);
      h_error_dat = CONCAT22(pvVar3,uVar2);
      mem_lock(uVar2,pvVar3);
      read(iVar1,(void *)CONCAT22((int)lVar4,pvVar3),unaff_DI);
      mem_unlock((int)h_error_dat,(int)((ulong)h_error_dat >> 0x10));
      close(iVar1);
      iVar1 = open(s_DATA_FILE_DAT_4125_26b3,2);
      if (-1 < iVar1) {
        lVar4 = filelength(iVar1);
        pvVar3 = (void *)((ulong)lVar4 >> 0x10);
        uVar2 = mem_alloc((long)(int)lVar4,0,0xa8,0);
        h_file_dat = CONCAT22(pvVar3,uVar2);
        mem_lock(uVar2,pvVar3);
        read(iVar1,(void *)CONCAT22((int)lVar4,pvVar3),unaff_DI);
        mem_unlock((int)h_file_dat,(int)((ulong)h_file_dat >> 0x10));
        close(iVar1);
      }
    }
  }
  return;
}


