
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_177b_011f(void)

{
  int iVar1;
  long local_6;
  
  iVar1 = FUN_7ecb_0a7c();
  if (iVar1 == 0) {
    _bios_timeofday(0,&local_6);
    if (local_6 != _LAB_4125_1247_1) {
      _LAB_4125_1247_1 = local_6;
      _LAB_4125_1259_1 = _LAB_4125_1259_1 + 1 & 7;
      FUN_177b_0161(_LAB_4125_1259_1);
    }
  }
  return;
}


