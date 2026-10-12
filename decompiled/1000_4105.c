
int __cdecl16far fseek(FILE *__stream,long __off,int __whence)

{
  char *pcVar1;
  int iVar2;
  long lVar3;
  undefined2 in_stack_00000006;
  
  iVar2 = fflush(__stream);
  if (iVar2 == 0) {
    if ((__whence == 1) && (0 < ___stream->_flags)) {
      iVar2 = __ftell_adjust();
      __off = __off - iVar2;
    }
    __stream->_IO_read_ptr = (char *)((uint)__stream->_IO_read_ptr & 0xfe5f);
    ___stream->_flags = 0;
    pcVar1 = __stream->_IO_write_base;
    __stream->_IO_buf_base = __stream->_IO_write_ptr;
    __stream->_IO_write_end = pcVar1;
    lVar3 = lseek((int)*(char *)&__stream->_IO_read_end,__off,__whence);
    if (lVar3 == -1) {
      iVar2 = -1;
    }
    else {
      iVar2 = 0;
    }
  }
  else {
    iVar2 = -1;
  }
  return iVar2;
}


