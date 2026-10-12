
void __cdecl16near __heap_join_free(void)

{
  undefined2 uVar1;
  int in_DX;
  int iVar2;
  int iStack_2;
  
  iStack_2 = *(int *)&DAT_4125_0002;
  *(undefined2 *)&DAT_4125_0002 = 0;
  *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 4) = iStack_2;
  if ((in_DX == DAT_1000_3196) || (*(int *)&DAT_4125_0002 != 0)) {
    __heap_link();
    iStack_2 = in_DX;
  }
  else {
    iVar2 = *(int *)0x0;
    *(int *)0x0 = *(int *)0x0 + iVar2;
    iVar2 = in_DX + iVar2;
    if (*(int *)&DAT_4125_0002 == 0) {
      *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 4) = iStack_2;
    }
    else {
      *(int *)&DAT_4125_0002 = iStack_2;
    }
  }
  iVar2 = iStack_2 + *(int *)0x0;
  if (*(int *)&DAT_4125_0002 != 0) {
    return;
  }
  *(int *)0x0 = *(int *)0x0 + *(int *)0x0;
  *(int *)&DAT_4125_0002 = iStack_2;
  if (iVar2 != *(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 2)) {
    uVar1 = *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 2);
    DAT_1000_319a = *(undefined2 *)(char *)s_Borland_C_____Copyright_1991_Bor_4125_0004;
    *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 2) = uVar1;
    *(undefined2 *)(char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 = DAT_1000_319a;
    return;
  }
  DAT_1000_319a = 0;
  return;
}


