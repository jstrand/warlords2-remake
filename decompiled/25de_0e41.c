
undefined2 __stdcall16far FUN_25de_0e41(undefined2 param_1,undefined2 param_2,int param_3)

{
  code *pcVar1;
  undefined2 uVar2;
  char extraout_AH;
  char extraout_AH_00;
  undefined1 extraout_AH_01;
  int extraout_DX;
  
  uVar2 = DAT_1f00_1009;
  if ((((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] & 1U) != 0) {
    if ((((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] & 2U) != 0) {
      return 0;
    }
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] =
         ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] | 2;
    *(undefined2 *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 5) = param_2;
    *(undefined2 *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 7) = param_1;
    if (param_3 == 0) {
      pcVar1 = (code *)swi(0x67);
      (*pcVar1)();
      if (extraout_AH != '\0') {
        return 0xffff;
      }
      ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] =
           ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] | 4;
      param_3 = extraout_DX;
    }
    *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 3) = param_3;
    pcVar1 = (code *)swi(0x67);
    (*pcVar1)();
    if (extraout_AH_00 == '\0') {
      pcVar1 = (code *)swi(0x67);
      (*pcVar1)();
      return CONCAT11(extraout_AH_01,extraout_AH_01);
    }
  }
  return 0xffff;
}


