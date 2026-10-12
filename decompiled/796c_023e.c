
void __cdecl16far FUN_796c_023e(void)

{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < 5; iVar1 = iVar1 + 1) {
    if (iVar1 < 4) {
      ((undefined2 *)&DAT_451b_08d0)[iVar1] = ((undefined2 *)&DAT_451b_08d2)[iVar1];
    }
    else {
      ((undefined2 *)&DAT_451b_08d0)[iVar1] = ((undefined2 *)&DAT_451b_08d0)[iVar1] + 1;
    }
  }
  DAT_451b_08da = DAT_451b_08da + -1;
  if (DAT_451b_08da < 0) {
    DAT_451b_08da = 0;
  }
  FUN_796c_00f3();
  FUN_796c_0301();
  return;
}


