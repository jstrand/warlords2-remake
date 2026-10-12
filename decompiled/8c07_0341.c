
undefined2 __cdecl16far FUN_8c07_0341(void)

{
  undefined2 uVar1;
  undefined2 in_DX;
  undefined4 local_6;
  
  FUN_828e_0af3();
  FUN_177b_003d();
  uVar1 = FUN_8c07_05c4();
  local_6 = CONCAT22(in_DX,uVar1);
  if (local_6 == 0) {
    DAT_451b_1f02 = 0;
    sound_chord();
  }
  else {
    FUN_8c07_06eb(local_6);
    sound_ding();
  }
  FUN_89e0_0356();
  FUN_8065_0174();
  return 1;
}


