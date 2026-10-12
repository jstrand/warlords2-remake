
undefined2 __cdecl16far mem_lock(undefined2 *param_1,int param_2,undefined2 param_3)

{
  undefined2 uVar1;
  bool bVar2;
  byte bVar3;
  
  uVar1 = 0x1d28;
  bVar2 = (*(byte *)(param_1 + 2) & 8) == 0;
  if (!bVar2) {
    bVar3 = (byte)param_1[2];
    FUN_1d28_0244(param_1[1],*(undefined2 *)CONCAT22(param_2,param_1));
    if (bVar2) {
      DAT_4125_3280 = 3;
      if (DAT_4125_3284 == (code *)0x0 && DAT_4125_3286 == 0) {
        DAT_4125_3280 = 3;
        return 0;
      }
      (*DAT_4125_3284)(0x1d28,param_2,param_3);
      return 0;
    }
    uVar1 = 0x1000;
    (*(code *)*(undefined2 *)((undefined2 *)&DAT_4125_3292)[bVar3 & 7])();
    param_1[1] = param_2;
    *(undefined2 *)CONCAT22(param_2,param_1) = 0;
    *(byte *)(param_1 + 2) = *(byte *)(param_1 + 2) & 0xf7;
    *(undefined2 **)(char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 = param_1;
  }
  if ((DAT_4125_3274 == param_2) && (param_1 < DAT_4125_3278)) {
    *(byte *)(param_1 + 2) = *(byte *)(param_1 + 2) | 0x20;
    ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] =
         ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] | 0x20;
  }
  else {
    DAT_4125_3280 = 4;
    if (DAT_4125_3284 != (code *)0x0 || DAT_4125_3286 != 0) {
      (*DAT_4125_3284)(uVar1,0,0);
    }
  }
  return 0;
}


