
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_8065_0e04(void)

{
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  if (_LAB_4125_1176 == 0) {
    DAT_451b_1e60 = DAT_3c04_017d;
    DAT_451b_1e62 = DAT_3c04_017f;
    FUN_22bf_0b0c(&local_8);
    DAT_451b_1e58 = 0;
    DAT_451b_1e5c = 0;
  }
  else {
    FUN_22bf_0b0c(&local_8);
    DAT_451b_1e58 = DAT_451b_1e58 - local_8;
    DAT_451b_1e5c = DAT_451b_1e5c - local_a / 2;
    local_4 = (int)(DAT_451b_1e58 / 0x28);
    local_6 = (int)(DAT_451b_1e5c / 0x28);
    if ((DAT_3c04_017d != DAT_451b_1e60 + local_4) || (DAT_3c04_017f != DAT_451b_1e62 + local_6)) {
      FUN_8611_0629(DAT_451b_1e60 + local_4,DAT_451b_1e62 + local_6);
    }
  }
  return;
}


