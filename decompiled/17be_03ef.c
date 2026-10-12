
undefined2 __cdecl16far FUN_17be_03ef(int param_1)

{
  undefined2 uVar1;
  
  if (DAT_4125_166a == 0) {
    uVar1 = 0;
  }
  else {
    if ((param_1 == 0x1b) || (param_1 == 0x153)) {
      DAT_451b_0cde = FUN_18a9_03f5(1);
      FUN_17be_0e26(0);
    }
    else if (param_1 != 0x181) {
      sound_chord();
    }
    uVar1 = 1;
  }
  return uVar1;
}


