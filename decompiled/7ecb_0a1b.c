
void __cdecl16far FUN_7ecb_0a1b(void)

{
  undefined2 uVar1;
  undefined2 in_DX;
  undefined4 local_6;
  
  DAT_4125_3286 = 0x40c1;
  DAT_4125_3284 = 0xa7;
  uVar1 = mem_alloc(150000,0,1);
  local_6 = CONCAT22(in_DX,uVar1);
  if (local_6 != 0) {
    uVar1 = mem_lock(uVar1,in_DX);
    FUN_1d28_048e(local_6,uVar1,in_DX);
  }
  DAT_4125_3286 = 0x1a4c;
  DAT_4125_3284 = 0x388;
  return;
}


