
undefined2 __stdcall16far FUN_25de_1081(uint param_1,int param_2,uint param_3,int param_4)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  
  uVar1 = DAT_1f00_1009;
  if ((((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0x13] & 1U) != 0) {
    if ((((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0x13] & 2U) != 0) {
      return 0;
    }
    ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0x13] =
         ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[0x13] | 2;
    if (*(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14) == 0 &&
        *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x16) == 0) {
      *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb) = param_3;
      *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xd) = param_4;
      DAT_3f09_002f = uRam00000064;
      DAT_3f09_0031 = uRam00000066;
      uRam00000064 = 0x3f;
      uRam00000066 = 0x3f09;
      DAT_3f09_003a = param_3;
      DAT_3f09_003c = (undefined1)param_4;
      iVar2 = param_3 + param_1;
      iVar3 = param_4 + param_2 + (uint)CARRY2(param_3,param_1);
      DAT_3f09_002e = (undefined1)iVar3;
      DAT_3f09_002c = iVar2;
LAB_25de_1144:
      *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xf) = iVar2;
      *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x11) = iVar3;
      return 0;
    }
    uVar4 = 0x400;
    uVar5 = (**(code **)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14))(0x2000);
    if ((int)uVar5 != 0) {
      *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x18) =
           (int)((ulong)uVar5 >> 0x10);
      uVar5 = (**(code **)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14))(0x2000);
      iVar3 = (int)((ulong)uVar5 >> 0x10);
      if ((int)uVar5 != 0) {
        *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb) = uVar4;
        *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xd) = iVar3;
        *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x1a) = uVar4;
        *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x1c) = iVar3;
        iVar2 = uVar4 + param_1;
        iVar3 = iVar3 + param_2 + (uint)CARRY2(uVar4,param_1);
        goto LAB_25de_1144;
      }
    }
  }
  return 0xffff;
}


