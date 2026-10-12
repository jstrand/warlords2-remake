
void __cdecl16near __heap_release_last(void)

{
  int iVar1;
  int in_DX;
  void *__addr;
  
  if (in_DX == DAT_1000_3196) {
LAB_1000_31e0:
    DAT_1000_3196 = 0;
    DAT_1000_3198 = 0;
    DAT_1000_319a = 0;
  }
  else {
    iVar1 = *(int *)&DAT_4125_0002;
    DAT_1000_3198 = iVar1;
    if (*(int *)&DAT_4125_0002 == 0) {
      if (iVar1 != DAT_1000_3196) {
        DAT_1000_3198 = *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 4);
        __addr = (void *)0x0;
        __heap_unlink(0,iVar1);
        goto LAB_1000_31fe;
      }
      goto LAB_1000_31e0;
    }
  }
  __addr = (void *)0x0;
LAB_1000_31fe:
  brk(__addr);
  return;
}


