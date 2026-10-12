
undefined2 __cdecl16far midpak_load_driver(undefined4 param_1)

{
  undefined2 uVar1;
  int iVar2;
  
  iVar2 = (int)param_1;
  uVar1 = dos_load_driver(iVar2,(int)((ulong)param_1 >> 0x10),0,0,0xd31,0x1000);
  if (iVar2 != 0) {
    DAT_1000_0d2d = uVar1;
    DAT_1000_0d2f = iVar2;
    if ((((*(char *)((int)(undefined2 *)&DAT_4125_0002 + 1) == 'M') &&
         (*(char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 == 'I')) &&
        (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[1] == 'D')) &&
       (((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[2] == 'P' &&
         (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[3] == 'A')) &&
        ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] == 'K' &&
         (DAT_1000_0d35 = DAT_1000_0d31 + -0x10, DAT_1000_0d39 = DAT_1000_0d31 + -0x10,
         iVar2 = (*DAT_4125_0d33)(0x1000), iVar2 == 0)))))) {
      uVar1 = 1;
    }
    else {
      FUN_255e_07ea(0,DAT_1000_0d31);
      uVar1 = 0;
      DAT_1000_0d31 = 0;
    }
  }
  return uVar1;
}


