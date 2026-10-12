
void * __cdecl16far farmalloc(ulong size)

{
  int *piVar1;
  ulong uVar2;
  void *pvVar3;
  uint uVar4;
  int iVar5;
  undefined2 extraout_DX;
  
  DAT_1000_319c = 0x4125;
  pvVar3 = (void *)0x0;
  if (size != 0) {
    uVar2 = size + 0x13;
    if ((CARRY2(size._2_2_,(uint)(0xffec < (uint)size))) || ((uVar2 & 0xfff00000) != 0)) {
      pvVar3 = (void *)0x0;
    }
    else {
      uVar4 = CONCAT11((byte)(uVar2 >> 8) >> 4 | (char)(uVar2 >> 0x10) * '\x10',
                       (char)((uint)uVar2 >> 4));
      if (DAT_1000_3196 == 0) {
        pvVar3 = (void *)__heap_create();
      }
      else {
        iVar5 = DAT_1000_319a;
        if (DAT_1000_319a != 0) {
          do {
            if (uVar4 <= *(uint *)0x0) {
              if (*(uint *)0x0 <= uVar4) {
                __heap_unlink();
                *(undefined2 *)&DAT_4125_0002 =
                     *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 4);
                return (char *)CONCAT22(extraout_DX,
                                        (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004);
              }
              pvVar3 = (void *)__heap_split();
              return pvVar3;
            }
            piVar1 = (int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 2);
            iVar5 = *piVar1;
          } while (*piVar1 != DAT_1000_319a);
        }
        pvVar3 = (void *)__heap_extend();
      }
    }
  }
  return pvVar3;
}


