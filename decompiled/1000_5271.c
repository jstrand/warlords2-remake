
void __cdecl16far __xfflush(void)

{
  int iVar1;
  FILE *local_6;
  
  local_6 = (FILE *)0x3b6c;
  for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
    if (((uint)local_6->_IO_read_ptr & 3) != 0) {
      fflush(local_6);
    }
    local_6 = (FILE *)&local_6->_IO_backup_base;
  }
  return;
}


