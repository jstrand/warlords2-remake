
/* WARNING: Removing unreachable block (ram,0x00013449) */

void * __cdecl16far malloc(uint size)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  undefined2 extraout_DX;
  void *pvVar4;
  
  DAT_1000_319c = 0x4125;
  pvVar4 = (void *)0x0;
  if (size != 0) {
    uVar2 = CONCAT11((byte)(size + 0x13 >> 0xc) | (0xffec < size) << 4,(char)(size + 0x13 >> 4));
    if (DAT_1000_3196 == 0) {
      pvVar4 = (void *)__heap_create();
    }
    else {
      iVar3 = DAT_1000_319a;
      if (DAT_1000_319a != 0) {
        do {
          if (uVar2 <= *(uint *)0x0) {
            if (*(uint *)0x0 <= uVar2) {
              __heap_unlink();
              *(undefined2 *)&DAT_4125_0002 =
                   *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 4);
              return (char *)CONCAT22(extraout_DX,
                                      (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004);
            }
            pvVar4 = (void *)__heap_split();
            return pvVar4;
          }
          piVar1 = (int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 2);
          iVar3 = *piVar1;
        } while (*piVar1 != DAT_1000_319a);
      }
      pvVar4 = (void *)__heap_extend();
    }
  }
  return pvVar4;
}


