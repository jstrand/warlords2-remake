
void __cdecl16far FUN_17be_0e26(int param_1)

{
  int iVar1;
  undefined4 local_6;
  
  local_6 = (int *)CONCAT22(((int *)DAT_451b_0ce0)[1],(int *)(*DAT_451b_0ce0 + DAT_451b_0cde * 0x21)
                           );
  iVar1 = FUN_18a9_05ab(*local_6);
  if (((iVar1 == 2) || (iVar1 = FUN_18a9_05ab(*local_6), iVar1 == 0)) ||
     (iVar1 = FUN_18a9_05ab(*local_6), iVar1 == 0)) {
    sound_chord();
    return;
  }
  FUN_18a9_066e(*local_6,0);
  if (param_1 != 0) {
    DAT_4125_166c = 1;
    DAT_451b_0cdc = DAT_451b_0cde;
    return;
  }
  if (*local_6 == 0) {
    return;
  }
  FUN_17be_04d8(0x18a9,*local_6,0);
  return;
}


