
void __cdecl16far FUN_89e0_157d(int param_1)

{
  int iVar1;
  undefined2 unaff_SI;
  undefined2 unaff_SS;
  int local_12 [8];
  
  if (((undefined2 *)&DAT_451b_28e2)[param_1] == 0) {
    for (iVar1 = 0; iVar1 < DAT_4125_30f6; iVar1 = iVar1 + 1) {
      if (((undefined2 *)&DAT_451b_28e2)[iVar1] != 0) {
        unaff_SI = ((undefined2 *)&DAT_451b_28f2)[iVar1];
        break;
      }
    }
    ((undefined2 *)&DAT_451b_28f2)[param_1] = unaff_SI;
    ((undefined2 *)&DAT_451b_28e2)[param_1] = (uint)(((undefined2 *)&DAT_451b_28e2)[param_1] == 0);
  }
  else {
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      local_12[iVar1] = 0;
    }
    for (iVar1 = 0; iVar1 < DAT_4125_30f6; iVar1 = iVar1 + 1) {
      local_12[((undefined2 *)&DAT_451b_28f2)[iVar1]] =
           local_12[((undefined2 *)&DAT_451b_28f2)[iVar1]] + 1;
    }
    if (local_12[((undefined2 *)&DAT_451b_28f2)[param_1]] != 1) {
      for (iVar1 = 0; (iVar1 < 8 && (local_12[iVar1] != 0)); iVar1 = iVar1 + 1) {
      }
      ((undefined2 *)&DAT_451b_28e2)[param_1] = (uint)(((undefined2 *)&DAT_451b_28e2)[param_1] == 0)
      ;
      ((undefined2 *)&DAT_451b_28f2)[param_1] = iVar1;
    }
  }
  FUN_89e0_17e7();
  FUN_89e0_0e85(0,0);
  FUN_89e0_1425();
  return;
}


