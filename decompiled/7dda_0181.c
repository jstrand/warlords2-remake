
void __cdecl16far sound_casualty(int defender)

{
  undefined2 uVar1;
  
  if (DAT_4125_0bec != 0) {
    uVar1 = DAT_451b_1a1e;
    if (defender == 0) {
      uVar1 = DAT_451b_1a14;
    }
    sample_play(uVar1,0);
  }
  return;
}


