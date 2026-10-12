
int __cdecl16far FUN_2372_0c98(int param_1)

{
  int iVar1;
  
  iVar1 = DAT_451b_3013;
  if ((DAT_451b_3013 != 0xff) &&
     ((*(ulong *)(*DAT_451b_2fc8 + param_1 * 0x2e + 0x1a) >> ((byte)DAT_451b_3013 & 0x1f) & 1) == 0)
     ) {
    iVar1 = FUN_2372_0c50(param_1);
  }
  return iVar1;
}


