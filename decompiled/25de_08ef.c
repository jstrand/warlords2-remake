
int __cdecl16far FUN_25de_08ef(int param_1,uint param_2,uint param_3)

{
  long lVar1;
  long lVar2;
  undefined2 uVar3;
  uint uVar4;
  int iVar5;
  undefined2 uVar6;
  
  uVar3 = DAT_1f00_1009;
  if ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0xc] & 1U) == 0) {
    if (param_1 == 0) {
      uVar4 = FUN_25de_0d8f(0x4125);
      if (uVar4 == 0) {
        return -1;
      }
      if (uVar4 <= param_2) {
        return -1;
      }
      if ((param_3 == 0) || (uVar4 - param_2 <= param_3)) {
        param_3 = uVar4 - param_2;
      }
    }
    else {
      if (param_3 == 0) {
        return -1;
      }
      if (param_3 <= param_2) {
        return -1;
      }
      ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] =
           ((char *)s_Divide_error_Abnormal_program_te_4125_002f)[9] | 1;
    }
    uVar4 = (uint)(CONCAT22(*(int *)0x3884 + (uint)(0xc000 < *(uint *)0x3882),
                            *(uint *)0x3882 + 0x3fff) / 0x4000);
    if (uVar4 < param_3) {
      param_3 = uVar4;
    }
    if ((3 < param_3) || (*(uint *)0x11a <= param_3 * 0x400)) {
      iVar5 = FUN_25de_0e41(param_3,param_2,param_1);
      if (iVar5 == 0) {
        lVar1 = (ulong)param_2 * 0x4000;
        uVar6 = (undefined2)((ulong)lVar1 >> 0x10);
        *(undefined2 *)0x140 = (int)lVar1;
        *(undefined2 *)0x142 = uVar6;
        lVar2 = (ulong)param_3 * 0x4000 + lVar1;
        *(undefined2 *)0x144 = (int)lVar2;
        *(undefined2 *)0x146 = (int)((ulong)lVar2 >> 0x10);
        *(undefined2 *)0x148 = (int)lVar1;
        *(undefined2 *)0x14a = uVar6;
        *(undefined2 *)0x14c = 0xa4f;
        *(undefined2 *)0x14e = 0;
        *(undefined2 *)0x112 = 1;
        ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0xc] =
             ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0xc] | 2;
        *(undefined2 *)((int)(undefined2 *)&DAT_4125_0083 + 1) = 0xea6;
        *(undefined2 *)((int)(undefined2 *)&DAT_4125_007f + 1) = 0xd15;
        return 0;
      }
      return iVar5;
    }
  }
  return -1;
}


