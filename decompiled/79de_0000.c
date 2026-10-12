
void __cdecl16far auto_str_fatal_error(undefined4 param_1)

{
  undefined2 uVar1;
  undefined2 in_DX;
  undefined1 local_a [8];
  
  FUN_24d0_0002();
  uVar1 = FUN_216d_002e(local_a);
  FUN_216d_0288(uVar1,in_DX);
  sound_chord();
  printf(s_fatal_error____s_4125_1656,param_1);
  FUN_7ecb_012b();
  auto_str_warlords_c_strategic_studies();
  return;
}


