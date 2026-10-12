
int __cdecl16far fflush(FILE *__stream)

{
  char *__buf;
  char *pcVar1;
  char *pcVar2;
  char *in_stack_00000006;
  
  if (__stream == (FILE *)0x0 && in_stack_00000006 == (char *)0x0) {
    flushall();
  }
  else {
    if ((FILE *)__stream->_IO_save_base != __stream) {
      return -1;
    }
    if (___stream->_flags < 0) {
      pcVar1 = __stream->_IO_read_base + ___stream->_flags + 1;
      ___stream->_flags = ___stream->_flags - (int)pcVar1;
      pcVar2 = __stream->_IO_write_ptr;
      __buf = __stream->_IO_write_base;
      __stream->_IO_buf_base = pcVar2;
      __stream->_IO_write_end = __buf;
      pcVar2 = (char *)write((int)*(char *)&__stream->_IO_read_end,__buf,
                             (size_t)CONCAT22(pcVar1,pcVar2));
      if ((pcVar2 != pcVar1) && (((uint)__stream->_IO_read_ptr & 0x200) == 0)) {
        __stream->_IO_read_ptr = (char *)((uint)__stream->_IO_read_ptr | 0x10);
        return -1;
      }
    }
    else if ((((uint)__stream->_IO_read_ptr & 8) != 0) ||
            ((__stream->_IO_buf_base == in_stack_00000006 &&
             (__stream->_IO_write_end == (char *)((int)&__stream->_IO_read_end + 1))))) {
      ___stream->_flags = 0;
      if ((__stream->_IO_buf_base == in_stack_00000006) &&
         (__stream->_IO_write_end == (char *)((int)&__stream->_IO_read_end + 1))) {
        pcVar2 = __stream->_IO_write_base;
        __stream->_IO_buf_base = __stream->_IO_write_ptr;
        __stream->_IO_write_end = pcVar2;
      }
    }
  }
  return 0;
}


