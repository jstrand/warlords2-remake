
undefined2 __cdecl16far FUN_7ecb_015e(int param_1)

{
  int iVar1;
  long local_6;
  
  iVar1 = FUN_7ecb_0a7c();
  if (iVar1 == 0) {
    _bios_timeofday();
    do {
      iVar1 = FUN_21b5_0003(0xd4,0x5998,0x4125);
      if (iVar1 != 0) {
        return 0x5998;
      }
      FUN_251d_008a();
      iVar1 = FUN_7ecb_0a7c();
    } while (((iVar1 == 0) && (iVar1 = _bios_timeofday(), iVar1 == 0)) &&
            (local_6 < local_6 + param_1));
  }
  return 0;
}


