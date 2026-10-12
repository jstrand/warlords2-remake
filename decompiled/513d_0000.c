
void __cdecl16far FUN_513d_0000(void)

{
  int iVar1;
  
  city_count = 0;
  FUN_513d_028c();
  build_army_move_flags();
  for (iVar1 = *(int *)((int)DAT_451b_002a + 0xa84); iVar1 < *(int *)((int)DAT_451b_002a + 0x2a);
      iVar1 = iVar1 + 1) {
    FUN_513d_0331(iVar1);
  }
  return;
}


