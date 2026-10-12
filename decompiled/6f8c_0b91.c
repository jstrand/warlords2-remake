
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_6f8c_0b91(void)

{
  int iVar1;
  
  FUN_6f8c_076d();
  iVar1 = (int)*(char *)((int)DAT_451b_0504 + 800);
  FUN_6f8c_07d3();
  DAT_451b_0500 = DAT_451b_0500 + 5;
  for (DAT_451b_0502 = DAT_451b_0502 + 5; (iVar1 <= DAT_451b_0500 && (0 < DAT_451b_0502));
      DAT_451b_0502 = DAT_451b_0502 + -1) {
    DAT_451b_0500 = DAT_451b_0500 + -1;
  }
  if (iVar1 + -1 <= DAT_451b_0500) {
    DAT_451b_0500 = iVar1 + -1;
  }
  _LAB_4125_0e51_1 = (DAT_451b_0500 - DAT_451b_0502) + 1;
  FUN_22bf_07a5();
  FUN_6f8c_086e(1);
  FUN_6ef3_026c();
  FUN_22bf_0757();
  return;
}


