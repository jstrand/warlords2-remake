
undefined2 __cdecl16far FUN_17be_0444(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  int *piVar3;
  
  if ((DAT_4125_2a86 != 0) && (DAT_4125_166a == 0)) {
    iVar2 = 0xd;
    piVar3 = (int *)0x4a4;
    do {
      if (*piVar3 == param_1) {
                    /* WARNING: Could not recover jumptable at 0x0001805b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        uVar1 = (*(code *)piVar3[0xd])();
        return uVar1;
      }
      piVar3 = piVar3 + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  return 0;
}


