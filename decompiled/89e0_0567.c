
undefined2 __cdecl16far FUN_89e0_0567(void)

{
  undefined2 uVar1;
  int iVar2;
  
  uVar1 = 1;
  if (DAT_451b_1f02 == 0) {
    uVar1 = 0;
  }
  else if (DAT_4125_30f6 == 1) {
    uVar1 = 1;
  }
  else {
    for (iVar2 = 0; iVar2 < DAT_4125_30f6; iVar2 = iVar2 + 1) {
      if (((undefined2 *)&DAT_451b_28e2)[iVar2] == 0) {
        return 0;
      }
    }
  }
  return uVar1;
}


