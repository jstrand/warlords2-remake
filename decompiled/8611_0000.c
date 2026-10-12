
void __cdecl16far auto_str_can_t_allocate_os(void)

{
  int iVar1;
  
  DAT_451b_2509 = DAT_451b_2509 & 0xfe;
  DAT_451b_2263 = DAT_451b_2263 & 0xfe;
  FUN_2012_09cf(0x6795,0x4125);
  iVar1 = FUN_2012_01fe(0x6795,0x4125,0x1680168);
  if (iVar1 == 0) {
    auto_str_fatal_error((char *)s_Can_t_allocate_OS_bitmap_4125_2f1c,0x4125);
  }
  auto_file_stand_pck();
  return;
}


