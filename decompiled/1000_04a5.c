
void __cdecl16far digpak_unload_driver(void)

{
  if (UINT_1000_03eb != 0) {
    (*(code *)s_A_Signpost__4125_03eb._6_2_)(0x1000);
    FUN_255e_07ea(UINT_1000_03e7,UINT_1000_03e9);
    UINT_1000_03eb = 0;
  }
  return;
}


