
void __cdecl16far FUN_6d51_0908(int param_1)

{
  float fVar1;
  int iVar2;
  int iVar3;
  int local_6 [2];
  
  if (param_1 == 0) {
    FUN_22bf_0af4(local_6);
    if (DAT_4125_0dc4 == 1) {
      fVar1 = (float)local_6[0] - (float)DAT_4125_0dd6;
      iVar2 = DAT_4125_0dda;
    }
    else {
      fVar1 = (float)local_6[0] - (float)DAT_4125_0dc6;
      iVar2 = DAT_4125_0dca;
    }
    iVar3 = __ftol(DAT_451b_04dc + -1,fVar1 / (float)iVar2);
    iVar2 = iVar3 + 1;
    if (iVar3 + 1 == DAT_451b_04de) {
      return;
    }
  }
  else {
    iVar2 = DAT_451b_04de + param_1;
  }
  DAT_451b_04de = iVar2;
  if (DAT_4125_0dc4 != 1) {
    FUN_24d0_0002();
    FUN_2133_0095(DAT_451b_04c8,DAT_451b_04cc,DAT_451b_04d0);
  }
  auto_ui_history_lines(0x10000);
  FUN_6d51_084e();
  return;
}


