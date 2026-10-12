
void __cdecl16near __heap_unlink(void)

{
  undefined2 uVar1;
  
  uVar1 = s_Borland_C_____Copyright_1991_Bor_4125_0004._2_2_;
  DAT_1000_319a = s_Borland_C_____Copyright_1991_Bor_4125_0004._0_2_;
  if (s_Borland_C_____Copyright_1991_Bor_4125_0004._2_2_ != 0x4125) {
    *(undefined2 *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 2) =
         s_Borland_C_____Copyright_1991_Bor_4125_0004._2_2_;
    *(undefined2 *)(char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 = DAT_1000_319a;
    return;
  }
  DAT_1000_319a = 0;
  return;
}


