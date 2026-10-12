
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far FUN_1ea3_008c(int param_1)

{
  code *pcVar1;
  char cVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 extraout_DX;
  undefined2 extraout_DX_00;
  char in_BL;
  bool bVar6;
  
  pcVar1 = (code *)swi(0x10);
  cVar2 = (*pcVar1)();
  uVar5 = 1;
  if ((cVar2 == '\x1a') && ((in_BL == '\a' || (in_BL == '\b')))) {
    DAT_4125_3388 = 1;
  }
  uVar3 = DAT_4125_3388;
  if (param_1 == 0) {
LAB_1ea3_0134:
    DAT_4125_3388 = uVar3;
    uVar3 = 0;
    DAT_4125_3382 = uVar5;
    DAT_4125_3384 = uVar5;
    goto LAB_1ea3_01a7;
  }
  if (param_1 == 1) {
    DAT_4125_3384 = 1;
    DAT_4125_3382 = 1;
    uVar3 = 1;
    goto LAB_1ea3_01a7;
  }
  if (param_1 == 2) {
    DAT_4125_3384 = 1;
    DAT_4125_3382 = 1;
    uVar3 = 2;
    goto LAB_1ea3_01a7;
  }
  if (param_1 == 4) {
    DAT_4125_3384 = 1;
    DAT_4125_3382 = 1;
    uVar3 = 4;
    goto LAB_1ea3_01a7;
  }
  if (param_1 == 10) {
    uVar3 = 10;
    DAT_4125_338c = 1;
    goto LAB_1ea3_01a7;
  }
  if (param_1 == 3) {
    DAT_4125_3386 = 1;
    DAT_4125_3382 = 1;
    return 3;
  }
  if (param_1 == 10) {
LAB_1ea3_0150:
    DAT_4125_337c = uVar5;
    DAT_4125_3378 = DAT_4125_3378 + -1;
    if (DAT_4125_3376 < 0xf1) {
      DAT_4125_337a = uVar5;
    }
    if ((0x16f < DAT_4125_3376) && (param_1 != 10)) {
      DAT_4125_3382 = uVar5;
      DAT_4125_3380 = uVar5;
      return 10;
    }
  }
  else {
    uVar5 = DAT_4125_337e;
    if (param_1 != 10) {
      if (param_1 != 10) {
        pcVar1 = (code *)swi(0x10);
        cVar2 = (*pcVar1)();
        if (cVar2 == '\x1a') {
          uVar5 = extraout_DX;
          uVar3 = extraout_DX;
          if ((in_BL == '\a') || (uVar3 = extraout_DX, in_BL == '\b')) goto LAB_1ea3_0134;
          if (in_BL == '\n') {
            DAT_4125_3382 = extraout_DX;
            DAT_4125_3386 = extraout_DX;
            return 3;
          }
          if (in_BL == '\v') {
            DAT_4125_3382 = extraout_DX;
            DAT_4125_3386 = extraout_DX;
            return 3;
          }
          if (in_BL == '\f') {
            DAT_4125_3382 = extraout_DX;
            DAT_4125_3386 = extraout_DX;
            return 3;
          }
        }
        iVar4 = 0;
        pcVar1 = (code *)swi(0x10);
        (*pcVar1)();
        uVar5 = extraout_DX_00;
        uVar3 = DAT_4125_3388;
        if (iVar4 != 0) goto LAB_1ea3_0134;
        bVar6 = (byte)_DAT_f000_c000 < 0x21;
        if ((byte)_DAT_f000_c000 == 0x21) goto LAB_1ea3_0150;
        FUN_1ea3_01b8();
        uVar5 = DAT_4125_337e;
        if (!bVar6) goto LAB_1ea3_019e;
      }
      DAT_4125_338a = 1;
      uVar3 = 10;
      goto LAB_1ea3_01a7;
    }
  }
LAB_1ea3_019e:
  DAT_4125_337e = uVar5;
  uVar3 = 10;
  DAT_4125_338c = 1;
LAB_1ea3_01a7:
  if (6 < DAT_4125_3378) {
    DAT_4125_3378 = 7;
  }
  return uVar3;
}


