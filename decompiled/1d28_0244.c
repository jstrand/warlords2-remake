
void __cdecl16near FUN_1d28_0244(void)

{
  undefined2 uVar1;
  undefined1 in_CF;
  undefined1 in_ZF;
  
  do {
    uVar1 = FUN_1d28_053c();
    if (!(bool)in_ZF) {
      return;
    }
    FUN_1d28_02cf(uVar1);
    uVar1 = FUN_1d28_053c();
    if (!(bool)in_ZF) {
      return;
    }
    FUN_1d28_05c8(uVar1);
    if (!(bool)in_ZF) {
      uVar1 = FUN_1d28_053c();
      if (!(bool)in_ZF) {
        return;
      }
      FUN_1d28_02cf(uVar1);
      uVar1 = FUN_1d28_053c();
      if (!(bool)in_ZF) {
        return;
      }
    }
    FUN_1d28_0a84(uVar1);
    if ((bool)in_CF) {
      DAT_4125_3280 = 2;
      return;
    }
    in_CF = 0;
  } while( true );
}


