
undefined2 __cdecl16far FUN_7bab_1241(int param_1)

{
  int iVar1;
  
  iVar1 = 0;
  while( true ) {
    if (5 < iVar1) {
      return 1;
    }
    if (((undefined2 *)&DAT_4125_2378)[iVar1 + param_1 * 10] !=
        ((undefined2 *)&DAT_4125_23b4)[iVar1]) break;
    iVar1 = iVar1 + 1;
  }
  return 0;
}


