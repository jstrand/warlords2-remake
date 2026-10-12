
void __cdecl16far FUN_6c1b_0620(void)

{
  undefined2 uVar1;
  
  FUN_18a9_066e(0x100f2);
  if (DAT_4125_0d68 < 2) {
    FUN_18a9_066e(0x200f3);
    uVar1 = 2;
  }
  else {
    FUN_18a9_066e(0x100f3);
    uVar1 = 1;
  }
  FUN_18a9_066e(0xf4,uVar1);
  if (DAT_451b_04a8 < 0) {
    uVar1 = 2;
  }
  else {
    uVar1 = 1;
  }
  FUN_18a9_066e(0xf5,uVar1);
  if (DAT_451b_04ac < 0) {
    uVar1 = 2;
  }
  else {
    uVar1 = 1;
  }
  FUN_18a9_066e(0xf6,uVar1);
  if (DAT_4125_0d6c == 0) {
    FUN_18a9_066e(0x100f9);
    FUN_18a9_066e(0xfa);
    if (DAT_451b_04aa < 0) {
      uVar1 = 2;
    }
    else {
      uVar1 = 1;
    }
    FUN_18a9_066e(0xf7,uVar1);
  }
  else {
    FUN_18a9_066e(0xf9);
    FUN_18a9_066e(0x100fa);
    if (DAT_451b_04aa < 0) {
      uVar1 = 2;
    }
    else {
      uVar1 = 1;
    }
    FUN_18a9_066e(0xf8,uVar1);
  }
  FUN_18a9_03b3();
  FUN_18a9_07ed();
  FUN_18a9_07ed();
  return;
}


