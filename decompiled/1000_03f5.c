
uint __cdecl16far digpak_load_driver(undefined4 param_1)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = (uint)param_1;
  uVar1 = dos_load_driver(uVar3,(int)((ulong)param_1 >> 0x10),0,0,0x3eb,0x1000);
  if (uVar3 != 0) {
    UINT_1000_03e7 = uVar1;
    UINT_1000_03e9 = uVar3;
    if ((((*(char *)((int)(undefined2 *)&DAT_4125_0002 + 1) == 'D') &&
         (*(char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 == 'I')) &&
        (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[1] == 'G')) &&
       (((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[2] == 'P' &&
         (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[3] == 'A')) &&
        ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] == 'K' &&
         (UINT_1000_03ef = UINT_1000_03eb - 0x10, DAT_1000_03f3 = UINT_1000_03eb - 0x10,
         iVar2 = (*(code *)s_A_Signpost__4125_03eb._2_2_)(0x1000), iVar2 == 0)))))) {
      uVar1 = 1;
    }
    else {
      FUN_255e_07ea(0,UINT_1000_03eb);
      uVar1 = 0;
      UINT_1000_03eb = 0;
    }
  }
  return uVar1;
}


