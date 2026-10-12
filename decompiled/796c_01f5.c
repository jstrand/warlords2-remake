
void __cdecl16far FUN_796c_01f5(void)

{
  int iVar1;
  int iVar2;
  
  iVar1 = 5;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if (iVar2 < 1) {
      ((undefined2 *)&DAT_451b_08d0)[iVar2] = ((undefined2 *)&DAT_451b_08d0)[iVar2] + -1;
    }
    else {
      ((undefined2 *)&DAT_451b_08d0)[iVar2] = ((undefined2 *)&DAT_451b_08ce)[iVar2];
    }
  }
  DAT_451b_08da = DAT_451b_08da + 1;
  if (4 < DAT_451b_08da) {
    DAT_451b_08da = 4;
  }
  FUN_796c_00f3();
  FUN_796c_0301();
  return;
}


