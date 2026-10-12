
int __cdecl16far setvbuf(FILE *__stream,char *__buf,int __modes,size_t __n)

{
  int iVar1;
  char *pcVar2;
  char *in_stack_0000000a;
  
  if ((((FILE *)__stream->_IO_save_base == __stream) && ((int)__n < 3)) && (__n < 0x80000000)) {
    if ((DAT_4125_3ed8 == 0) && (__stream == (FILE *)0x3b80)) {
      DAT_4125_3ed8 = 1;
    }
    else if ((DAT_4125_3ed6 == 0) && (__stream == (FILE *)0x3b6c)) {
      DAT_4125_3ed6 = 1;
    }
    if (___stream->_flags != 0) {
      fseek(__stream,0,1);
    }
    if (((uint)__stream->_IO_read_ptr & 4) != 0) {
      FUN_1d28_050d();
    }
    __stream->_IO_read_ptr = (char *)((uint)__stream->_IO_read_ptr & 0xfff3);
    __stream->_IO_read_base = (char *)0x0;
    pcVar2 = (char *)((int)&__stream->_IO_read_end + 1);
    __stream->_IO_write_ptr = __buf;
    __stream->_IO_write_base = pcVar2;
    __stream->_IO_buf_base = __buf;
    __stream->_IO_write_end = pcVar2;
    if (((int)__n != 2) && (__n._2_2_ != (char *)0x0)) {
      DAT_4125_3b62 = 0x1000;
      DAT_4125_3b60 = 0x5271;
      if (__modes == 0 && in_stack_0000000a == (char *)0x0) {
        __modes = FUN_1d28_01d5(__n._2_2_);
        if (__modes == 0 && pcVar2 == (char *)0x0) goto LAB_1000_4809;
        __stream->_IO_read_ptr = (char *)((uint)__stream->_IO_read_ptr | 4);
        in_stack_0000000a = pcVar2;
      }
      __stream->_IO_buf_base = in_stack_0000000a;
      __stream->_IO_write_end = (char *)__modes;
      __stream->_IO_write_ptr = in_stack_0000000a;
      __stream->_IO_write_base = (char *)__modes;
      __stream->_IO_read_base = __n._2_2_;
      if ((int)__n == 1) {
        __stream->_IO_read_ptr = (char *)((uint)__stream->_IO_read_ptr | 8);
      }
    }
    iVar1 = 0;
  }
  else {
LAB_1000_4809:
    iVar1 = -1;
  }
  return iVar1;
}


