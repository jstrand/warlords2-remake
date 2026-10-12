
void __cdecl16far FUN_7bab_1f23(int param_1)

{
  int iVar1;
  
  if (param_1 == -1) {
    iVar1 = DAT_451b_19fa;
    if (5 < DAT_451b_19fa) {
      iVar1 = 5;
    }
    iVar1 = -iVar1;
  }
  else if ((DAT_451b_08ce - DAT_451b_1a02) + -1 < 6) {
    iVar1 = (DAT_451b_08ce - DAT_451b_1a02) + -1;
  }
  else {
    iVar1 = 5;
  }
  DAT_451b_19fa = DAT_451b_19fa + iVar1;
  for (iVar1 = 1; iVar1 < 5; iVar1 = iVar1 + 1) {
    ((undefined2 *)&DAT_451b_19fa)[iVar1] = ((undefined2 *)&DAT_451b_19f8)[iVar1] + 1;
  }
  FUN_7bab_1be6(1);
  FUN_7bab_1ae4();
  return;
}


