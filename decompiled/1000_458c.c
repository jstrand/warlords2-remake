
/* WARNING: Type propagation algorithm not settling */

char * __fputn(FILE *param_1,char *param_2,byte *param_3)

{
  byte bVar1;
  char *pcVar2;
  byte *pbVar3;
  byte *pbVar4;
  int iVar5;
  uint uVar6;
  char *pcVar7;
  FILE *__stream;
  undefined2 uVar8;
  
  uVar8 = (undefined2)((ulong)param_1 >> 0x10);
  __stream = (FILE *)param_1;
  pcVar7 = param_2;
  if (((uint)__stream->_IO_read_ptr & 8) == 0) {
    if (((uint)__stream->_IO_read_ptr & 0x40) == 0) {
      if (__stream->_IO_read_base != (char *)0x0) {
        do {
          if (pcVar7 == (char *)0x0) {
            return param_2;
          }
          iVar5 = param_1->_flags;
          param_1->_flags = param_1->_flags + 1;
          if (SCARRY2(iVar5,1) == param_1->_flags < 0) {
            uVar6 = _fputc(*param_3,__stream,uVar8);
          }
          else {
            pcVar2 = __stream->_IO_buf_base;
            pbVar3 = (byte *)__stream->_IO_write_end;
            __stream->_IO_write_end = __stream->_IO_write_end + 1;
            bVar1 = *param_3;
            *pbVar3 = bVar1;
            uVar6 = (uint)bVar1;
          }
          param_3 = (byte *)CONCAT22(param_3._2_2_,(byte *)param_3 + 1);
          pcVar7 = pcVar7 + -1;
        } while (uVar6 != 0xffff);
        return (char *)0x0;
      }
      pcVar7 = (char *)write((int)*(char *)&__stream->_IO_read_end,(byte *)param_3,
                             CONCAT22(param_2,param_3._2_2_));
    }
    else if (__stream->_IO_read_base == (char *)0x0) {
      if ((*(uint *)(*(char *)&__stream->_IO_read_end * 2 + 0x3cfe) & 0x800) != 0) {
        lseek((int)*(char *)&__stream->_IO_read_end,0,2);
      }
      pcVar7 = (char *)_write();
    }
    else {
      if (param_2 <= __stream->_IO_read_base) {
        if (SCARRY2(param_1->_flags,(int)param_2) == (int)(param_2 + param_1->_flags) < 0) {
          if (param_1->_flags == 0) {
            param_1->_flags = -1 - (int)__stream->_IO_read_base;
          }
          else {
            iVar5 = fflush(__stream);
            if (iVar5 != 0) {
              return (char *)0x0;
            }
          }
        }
        memcpy((char *)CONCAT22(__stream->_IO_buf_base,__stream->_IO_write_end),param_3,
               (uint)param_2);
        param_1->_flags = (int)(param_2 + param_1->_flags);
        __stream->_IO_write_end = __stream->_IO_write_end + (int)param_2;
        return param_2;
      }
      if ((param_1->_flags != 0) && (iVar5 = fflush(__stream), iVar5 != 0)) {
        return (char *)0x0;
      }
      if ((*(uint *)(*(char *)&__stream->_IO_read_end * 2 + 0x3cfe) & 0x800) != 0) {
        lseek((int)*(char *)&__stream->_IO_read_end,0,2);
      }
      pcVar7 = (char *)_write();
    }
    if (pcVar7 == param_2) {
      return param_2;
    }
  }
  else {
    do {
      pbVar4 = param_3;
      if (pcVar7 == (char *)0x0) {
        return param_2;
      }
      param_3 = (byte *)CONCAT22(param_3._2_2_,(byte *)param_3 + 1);
      iVar5 = fputc((int)(char)*pbVar4,__stream);
      pcVar7 = pcVar7 + -1;
    } while (iVar5 != -1);
  }
  return (char *)0x0;
}


