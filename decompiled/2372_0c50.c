
int __cdecl16far FUN_2372_0c50(int param_1)

{
  int iVar1;
  
  iVar1 = 1;
  while( true ) {
    if (0x1e < iVar1) {
      return 0xff;
    }
    if ((*(ulong *)(*DAT_451b_2fc8 + param_1 * 0x2e + 0x1a) >> ((byte)iVar1 & 0x1f) & 1) != 0)
    break;
    iVar1 = iVar1 + 1;
  }
  return iVar1;
}


