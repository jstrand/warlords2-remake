
int __cdecl16near FUN_25de_11ad(void)

{
  code *pcVar1;
  undefined2 uVar2;
  uint in_AX;
  int iVar3;
  uint in_CX;
  uint in_DX;
  uint uVar4;
  uint in_BX;
  uint unaff_SI;
  int unaff_DI;
  bool bVar5;
  byte bVar6;
  
  uVar2 = DAT_1f00_1009;
  if (*(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x18) == 0) {
    bVar6 = false;
    *(uint *)&DAT_4125_005f = in_AX;
    *(byte *)&DAT_4125_0061 = (byte)in_DX;
    *(uint *)&DAT_4125_0067 = in_BX;
    *(byte *)&DAT_4125_0069 = (byte)in_CX;
    pcVar1 = (code *)swi(0x15);
    (*pcVar1)(0x4125);
    if (!(bool)bVar6) {
      return 0;
    }
  }
  else {
    *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x1e) = unaff_SI + 1 & 0xfffe;
    *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x20) =
         unaff_DI + (uint)(0xfffe < unaff_SI);
    if ((byte)in_DX < 0x10) {
      ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x22)[0] = '\0';
      ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x22)[1] = '\0';
      uVar4 = in_DX >> 4 | in_DX << 0xc;
    }
    else {
      *(undefined2 *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x22) =
           *(undefined2 *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x18);
      bVar5 = in_AX < *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x1a);
      in_AX = in_AX - *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x1a);
      uVar4 = (in_DX - *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x1c)) -
              (uint)bVar5;
    }
    *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x24) = in_AX;
    *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x26) = uVar4;
    if ((byte)in_CX < 0x10) {
      ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x28)[0] = '\0';
      ((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x28)[1] = '\0';
      uVar4 = in_CX >> 4 | in_CX << 0xc;
    }
    else {
      *(undefined2 *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x28) =
           *(undefined2 *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x18);
      bVar5 = in_BX < *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x1a);
      in_BX = in_BX - *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x1a);
      uVar4 = (in_CX - *(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x1c)) -
              (uint)bVar5;
    }
    *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x2a) = in_BX;
    *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x2c) = uVar4;
    iVar3 = (**(code **)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x14))(0x2000);
    if (iVar3 != 0) {
      return 0;
    }
    bVar6 = 1;
  }
  return -(uint)bVar6;
}


