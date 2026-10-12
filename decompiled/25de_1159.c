
void __cdecl16far FUN_25de_1159(void)

{
  undefined2 uVar1;
  
  uVar1 = DAT_1f00_1009;
  if (((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0x13] != '\0') {
    if (*(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x18) == 0) {
      if (iRam00000066 == 0x3f09) {
        uRam00000064 = DAT_3f09_002f;
        iRam00000066 = DAT_3f09_0031;
        ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0x13] = '\0';
      }
    }
    else {
      (**(code **)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14))(0x2000,0x4125);
      (**(code **)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14))(0x2000);
    }
  }
  return;
}


