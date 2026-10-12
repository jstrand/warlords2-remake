
void __cdecl16far FUN_7ecb_0000(int param_1)

{
  int iVar1;
  long local_a;
  long local_6;
  
  iVar1 = FUN_7ecb_0a7c();
  if (iVar1 == 0) {
    _bios_timeofday(0,&local_6);
    local_a = local_6;
    do {
      if (local_6 + param_1 <= local_a) {
        return;
      }
      iVar1 = FUN_7ecb_0a7c();
      if (iVar1 != 0) {
        return;
      }
      iVar1 = _bios_timeofday(0,&local_a);
    } while (iVar1 == 0);
  }
  return;
}


