
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl16far auto_str_palloc_out_of_hdrs(void)

{
  int iVar1;
  undefined2 uVar2;
  long local_a;
  undefined4 local_6;
  
  local_a = CONCAT22((int)((ulong)_DAT_4125_33d2 >> 0x10),_DAT_4125_33d2 - 0xeU);
  if (_DAT_4125_33d2 - 0xeU < 0x6882) {
    local_a = 0x4125698c;
  }
  local_6 = _DAT_4125_33d2;
  while( true ) {
    if (0x698c < (uint)local_6) {
      local_6 = 0x41256882;
    }
    uVar2 = (undefined2)((ulong)local_6 >> 0x10);
    iVar1 = (int)local_6;
    if (*(char *)(iVar1 + 8) == '\0') break;
    if (local_6 == local_a) {
      FUN_251d_0099((char *)s_palloc__out_of_hdrs_4125_33e5,0x4125);
    }
    local_6 = CONCAT22(uVar2,iVar1 + 0xe);
  }
  *(undefined1 *)(iVar1 + 8) = 1;
  _DAT_4125_33d2 = CONCAT22(uVar2,iVar1 + 0xe);
  return iVar1;
}


