
void __cdecl16near __heap_link(void)

{
  char *pcVar1;
  undefined2 uVar2;
  
  s_Borland_C_____Copyright_1991_Bor_4125_0004._0_2_ = DAT_1000_319a;
  if (DAT_1000_319a != 0) {
    uVar2 = *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 2);
    pcVar1 = (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 2;
    pcVar1[0] = '%';
    pcVar1[1] = 'A';
    ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0] = '%';
    ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[1] = 'A';
    s_Borland_C_____Copyright_1991_Bor_4125_0004[2] = (char)uVar2;
    s_Borland_C_____Copyright_1991_Bor_4125_0004[3] = SUB21(uVar2,1);
    return;
  }
  DAT_1000_319a = 0x4125;
  builtin_strncpy(&ram0x00001294,"%A%A",4);
  return;
}


