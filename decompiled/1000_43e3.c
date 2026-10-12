
int __cdecl16far fputc(int __c,FILE *__stream)

{
  char *pcVar1;
  byte *pbVar2;
  int iVar3;
  undefined2 in_stack_00000008;
  
  DAT_451b_31fc = (byte)__c;
  if (___stream->_flags < -1) {
    ___stream->_flags = ___stream->_flags + 1;
    pcVar1 = __stream->_IO_buf_base;
    pbVar2 = (byte *)__stream->_IO_write_end;
    __stream->_IO_write_end = __stream->_IO_write_end + 1;
    *pbVar2 = DAT_451b_31fc;
    if ((((uint)__stream->_IO_read_ptr & 8) == 0) ||
       ((DAT_451b_31fc != 10 && (DAT_451b_31fc != 0xd)))) goto LAB_1000_4570;
    iVar3 = fflush(__stream);
joined_r0x0001443c:
    if (iVar3 == 0) {
LAB_1000_4570:
      return (uint)DAT_451b_31fc;
    }
  }
  else {
    if ((((uint)__stream->_IO_read_ptr & 0x90) == 0) && (((uint)__stream->_IO_read_ptr & 2) != 0)) {
      __stream->_IO_read_ptr = (char *)((uint)__stream->_IO_read_ptr | 0x100);
      if (__stream->_IO_read_base != (char *)0x0) {
        if ((___stream->_flags != 0) && (iVar3 = fflush(__stream), iVar3 != 0)) {
          return -1;
        }
        ___stream->_flags = -(int)__stream->_IO_read_base;
        pcVar1 = __stream->_IO_buf_base;
        pbVar2 = (byte *)__stream->_IO_write_end;
        __stream->_IO_write_end = __stream->_IO_write_end + 1;
        *pbVar2 = DAT_451b_31fc;
        if ((((uint)__stream->_IO_read_ptr & 8) == 0) ||
           ((DAT_451b_31fc != 10 && (DAT_451b_31fc != 0xd)))) goto LAB_1000_4570;
        iVar3 = fflush(__stream);
        goto joined_r0x0001443c;
      }
      if ((*(uint *)(*(char *)&__stream->_IO_read_end * 2 + 0x3cfe) & 0x800) != 0) {
        lseek((int)*(char *)&__stream->_IO_read_end,0,2);
      }
      if (((((DAT_451b_31fc != 10) || (((uint)__stream->_IO_read_ptr & 0x40) != 0)) ||
           (iVar3 = _write((int)*(char *)&__stream->_IO_read_end,0x3ed4,0x4125,1), iVar3 == 1)) &&
          (iVar3 = _write((int)*(char *)&__stream->_IO_read_end,(undefined1 *)&DAT_451b_31fc,0x4125,
                          1), iVar3 == 1)) || (((uint)__stream->_IO_read_ptr & 0x200) != 0))
      goto LAB_1000_4570;
    }
    __stream->_IO_read_ptr = (char *)((uint)__stream->_IO_read_ptr | 0x10);
  }
  return -1;
}


