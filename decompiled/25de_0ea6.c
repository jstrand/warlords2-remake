
void __cdecl16far FUN_25de_0ea6(void)

{
  code *pcVar1;
  undefined2 uVar2;
  
  uVar2 = DAT_1f00_1009;
  if (((((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] & 4U) != 0) &&
     (*(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 3) != -1)) {
    pcVar1 = (code *)swi(0x67);
    (*pcVar1)();
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 3)[0] = -1;
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 3)[1] = -1;
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] = '\0';
  }
  return;
}


