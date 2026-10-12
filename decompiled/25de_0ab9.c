
int __cdecl16far FUN_25de_0ab9(uint param_1,uint param_2,uint param_3,uint param_4)

{
  bool bVar1;
  undefined2 uVar2;
  int iVar3;
  uint uVar4;
  int in_DX;
  uint uVar5;
  
  uVar2 = DAT_1f00_1009;
  if ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0xc] & 2U) == 0) {
    bVar1 = false;
    iVar3 = FUN_25de_0ed1();
    if (iVar3 == 0 && in_DX == 0) {
LAB_25de_0ae0:
      iVar3 = -1;
    }
    else {
      while( true ) {
        if ((param_2 < *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xd)) ||
           ((param_2 == *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xd) &&
            (param_1 < *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb))))) {
          param_2 = *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xd);
          param_1 = *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xb);
        }
        if ((*(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x11) < param_2) ||
           ((*(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x11) <= param_2 &&
            (*(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xf) < param_1))))
        goto LAB_25de_0ae0;
        uVar4 = *(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xf) - param_1;
        uVar5 = (*(int *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0x11) - param_2) -
                (uint)(*(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xf) <
                      param_1);
        if ((param_3 != 0 || param_4 != 0) &&
           ((param_4 <= uVar5 &&
            ((param_4 < uVar5 ||
             (*(uint *)((char *)s_Divide_error_Abnormal_program_te_4125_002f + 0xf) < uVar4)))))) {
          uVar4 = param_3;
          uVar5 = param_4;
        }
        if ((*(uint *)0x3884 <= uVar5) && ((*(uint *)0x3884 < uVar5 || (*(uint *)0x3882 < uVar4))))
        {
          uVar5 = *(uint *)0x3884;
          uVar4 = *(uint *)0x3882;
        }
        if ((uVar5 == 0) && (uVar4 >> 4 < *(uint *)0x11a)) goto LAB_25de_0ae0;
        *(uint *)0x130 = param_1;
        *(uint *)0x132 = param_2;
        *(int *)0x134 = uVar4 + param_1;
        *(int *)0x136 = uVar5 + param_2 + (uint)CARRY2(uVar4,param_1);
        *(uint *)&DAT_4125_0138 = param_1;
        *(uint *)0x13a = param_2;
        iVar3 = FUN_25de_1081(uVar4,uVar5,param_1,param_2);
        if (iVar3 != 0) {
          return iVar3;
        }
        if (bVar1) break;
        bVar1 = true;
        param_3 = uVar4;
        param_4 = uVar5;
      }
      *(undefined2 *)0x13c = 0xc02;
      *(undefined2 *)0x13e = 0;
      *(undefined2 *)0x112 = 1;
      ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0xc] =
           ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0xc] | 1;
      *(undefined2 *)((int)(undefined2 *)&DAT_4125_0081 + 1) = (undefined1 *)&LAB_4125_1158_1;
      *(undefined2 *)((int)(undefined2 *)&DAT_4125_007f + 1) = 0xd15;
      iVar3 = 0;
    }
  }
  else {
    iVar3 = 0;
  }
  return iVar3;
}


