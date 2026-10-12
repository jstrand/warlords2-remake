
ssize_t __cdecl16far write(int __fd,void *__buf,size_t __n)

{
  uint *puVar1;
  char *pcVar2;
  uint uVar3;
  char *pcVar4;
  ssize_t sVar5;
  int iVar6;
  int iVar7;
  undefined2 unaff_SS;
  char local_90 [130];
  char *local_e;
  char local_9;
  int local_8;
  char *local_6;
  undefined2 uVar8;
  
  if ((uint)__fd < DAT_4125_3cfc) {
    if (__n._2_2_ + 1U < 2) {
      sVar5 = 0;
    }
    else {
      if ((*(uint *)(__fd * 2 + 0x3cfe) & 0x800) != 0) {
        lseek(__fd,0,2);
      }
      if ((*(uint *)(__fd * 2 + 0x3cfe) & 0x4000) == 0) {
        sVar5 = _write(__fd,__buf,__n);
      }
      else {
        puVar1 = (uint *)(__fd * 2 + 0x3cfe);
        *puVar1 = *puVar1 & 0xfdff;
        local_e = (char *)CONCAT22((int)__n,__buf);
        local_8 = __n._2_2_;
        do {
          _local_6 = (char *)CONCAT22(unaff_SS,local_90);
          do {
            pcVar4 = local_e;
            if (local_8 == 0) {
              iVar7 = (int)local_6 - (int)local_90;
              if (iVar7 == 0) {
                return __n._2_2_;
              }
              iVar6 = _write(__fd,local_90);
              if (iVar6 == iVar7) {
                return __n._2_2_;
              }
              if (iVar6 == -1) goto LAB_1000_51d4;
              goto LAB_1000_5226;
            }
            local_8 = local_8 + -1;
            local_e = (char *)CONCAT22(local_e._2_2_,(char *)local_e + 1);
            local_9 = *pcVar4;
            if (local_9 == '\n') {
              *_local_6 = '\r';
              uVar8 = (undefined2)((ulong)_local_6 >> 0x10);
              _local_6 = (char *)CONCAT22(uVar8,local_6 + 1);
            }
            *_local_6 = local_9;
            pcVar2 = local_6 + 1;
            uVar8 = (undefined2)((ulong)_local_6 >> 0x10);
            _local_6 = (char *)CONCAT22(uVar8,pcVar2);
            uVar3 = (uint)(pcVar2 < local_90);
          } while (((int)-uVar3 < 0) ||
                  ((uVar3 == 0 && ((uint)((int)pcVar2 - (int)local_90) < 0x80))));
          iVar7 = (int)pcVar2 - (int)local_90;
          iVar6 = _write(__fd,local_90);
        } while (iVar6 == iVar7);
        if (iVar6 == -1) {
LAB_1000_51d4:
          sVar5 = -1;
        }
        else {
          __n._2_2_ = __n._2_2_ - local_8;
LAB_1000_5226:
          sVar5 = (__n._2_2_ + iVar6) - iVar7;
        }
      }
    }
  }
  else {
    sVar5 = __IOerror(6);
  }
  return sVar5;
}


