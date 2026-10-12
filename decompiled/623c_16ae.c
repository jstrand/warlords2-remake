
undefined2 __cdecl16far FUN_623c_16ae(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar2 = param_1 + -1;
  do {
    if (param_1 + 1 < iVar2) {
      return 0;
    }
    for (iVar1 = param_2 + -1; iVar1 <= param_2 + 1; iVar1 = iVar1 + 1) {
      if ((((-1 < iVar2) && (-1 < iVar1)) && (iVar2 < 0x70)) &&
         ((iVar1 < 0x9c && ((*(byte *)(iVar1 * 0x70 + iVar2) & 0x20) != 0)))) {
        return 1;
      }
    }
    iVar2 = iVar2 + 1;
  } while( true );
}


