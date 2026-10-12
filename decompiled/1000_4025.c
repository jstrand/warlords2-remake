
int __cdecl16far flushall(void)

{
  int iVar1;
  int iVar2;
  FILE *local_6;
  
  iVar2 = 0;
  local_6 = (FILE *)0x3b6c;
  iVar1 = DAT_4125_3cfc;
  while (iVar1 != 0) {
    if (((uint)local_6->_IO_read_ptr & 3) != 0) {
      fflush(local_6);
      iVar2 = iVar2 + 1;
    }
    local_6 = (FILE *)&local_6->_IO_backup_base;
    iVar1 = iVar1 + -1;
  }
  return iVar2;
}


