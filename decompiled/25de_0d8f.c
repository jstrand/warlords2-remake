
int __cdecl16far FUN_25de_0d8f(void)

{
  code *pcVar1;
  char cVar2;
  char extraout_AH;
  undefined2 uVar3;
  char extraout_AH_00;
  char extraout_AH_01;
  uint extraout_DX;
  int in_BX;
  undefined2 unaff_SI;
  int unaff_ES;
  bool bVar4;
  
  bVar4 = false;
  if ((((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] & 1U) == 0) {
    pcVar1 = (code *)swi(0x21);
    in_BX = (*pcVar1)();
    if (bVar4) goto LAB_25de_0e33;
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    if ((!bVar4) && (bVar4 = false, (extraout_DX & 0x80) != 0)) {
      pcVar1 = (code *)swi(0x21);
      cVar2 = (*pcVar1)();
      if (!bVar4) {
        pcVar1 = (code *)swi(0x21);
        (*pcVar1)();
        if (cVar2 == -1) {
          pcVar1 = (code *)swi(0x21);
          (*pcVar1)();
          if (unaff_ES == 0 && in_BX == 0) goto LAB_25de_0e33;
          pcVar1 = (code *)swi(0x67);
          (*pcVar1)();
          if (extraout_AH != '\0') goto LAB_25de_0e33;
          ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] = '\x01';
          pcVar1 = (code *)swi(0x67);
          uVar3 = (*pcVar1)();
          if (((char)((uint)uVar3 >> 8) != '\0') || ((byte)uVar3 < 0x32)) goto LAB_25de_0e33;
          ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[10] = (byte)uVar3;
          pcVar1 = (code *)swi(0x67);
          (*pcVar1)();
          if (extraout_AH_00 != '\0') goto LAB_25de_0e33;
          *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 1) = in_BX;
          goto LAB_25de_0e1d;
        }
      }
    }
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
  }
  else {
    unaff_SI = DAT_1f00_1009;
    if ((((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] & 2U) != 0) {
      return *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 7);
    }
LAB_25de_0e1d:
    pcVar1 = (code *)swi(0x67);
    (*pcVar1)();
    if (extraout_AH_01 == '\0') {
      *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 7) = in_BX;
      ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 3)[0] = -1;
      ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 3)[1] = -1;
      return in_BX;
    }
  }
LAB_25de_0e33:
  ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] = '\0';
  return 0;
}


