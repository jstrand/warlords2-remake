
void __cdecl16far path_cache_reset_all(void)

{
  int iVar1;
  
  if (DAT_4125_00de != 0) {
    iVar1 = 0x14;
    while (iVar1 != 0) {
      path_cache_reset(iVar1 + -1);
      iVar1 = iVar1 + -1;
    }
  }
  return;
}


