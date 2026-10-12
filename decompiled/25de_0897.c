
void __cdecl16far FUN_25de_0897(undefined2 param_1,undefined2 param_2)

{
  undefined2 uVar1;
  
  uVar1 = 0x4125;
  if ((*(int *)&DAT_4125_0110 == *(int *)0x0) &&
     ((*(int *)((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 0xc) == 0 ||
      (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x1c] == -0x33)))) {
    FUN_25de_05a8();
    ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x16] =
         ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x16] & 0xf7;
    (*(code *)*(undefined2 *)0x86)(0x2000,param_2,uVar1);
  }
  return;
}


