
undefined4 __cdecl16near __heap_shrink(void)

{
  void *in_AX;
  int in_CX;
  int iVar1;
  int in_BX;
  int iVar2;
  
  if (in_BX != DAT_1000_3198) {
    iVar2 = in_BX + (int)in_AX;
    *(int *)0x0 = in_CX - (int)in_AX;
    *(int *)&DAT_4125_0002 = in_BX;
    *(int *)0x0 = (int)in_AX;
    iVar1 = in_BX + in_CX;
    if (*(int *)&DAT_4125_0002 == 0) {
      *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 4) = iVar2;
    }
    else {
      *(int *)&DAT_4125_0002 = iVar2;
    }
    free((void *)CONCAT22(iVar2,in_AX));
    return CONCAT22(in_BX,4);
  }
  *(int *)0x0 = (int)in_AX;
  brk((void *)0x0);
  return CONCAT22(in_BX,4);
}


