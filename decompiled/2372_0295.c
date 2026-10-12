
void __cdecl16far FUN_2372_0295(int param_1)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = 6;
  piVar2 = (int *)s_this_signpost__4125_0410;
  do {
    if (*piVar2 == param_1) {
                    /* WARNING: Could not recover jumptable at 0x000239d6. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*(code *)piVar2[6])();
      return;
    }
    piVar2 = piVar2 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}


