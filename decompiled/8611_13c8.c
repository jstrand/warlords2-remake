
void __cdecl16far FUN_8611_13c8(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  if (opt_hidden_map != 0) {
    DAT_451b_1f72 = 0;
    iVar5 = DAT_3c04_0177;
    if (-1 < param_1) {
      iVar5 = param_1;
    }
    iVar3 = DAT_3c04_0175;
    if (-1 < param_2) {
      iVar3 = param_2;
    }
    iVar1 = DAT_3c04_017b;
    if (-1 < param_1) {
      iVar1 = param_1;
    }
    iVar2 = DAT_3c04_0179;
    if (-1 < param_2) {
      iVar2 = param_2;
    }
    if (DAT_4125_3254 == 0) {
      for (; iVar6 = iVar3, iVar5 < iVar1 + 1; iVar5 = iVar5 + 1) {
        for (; iVar6 < iVar2 + 1; iVar6 = iVar6 + 1) {
          if ((*(byte *)(iVar6 * 0x70 + iVar5) & 0x20) == 0) {
            iVar4 = FUN_8611_0f6f(iVar5,iVar6);
            ((undefined1 *)&DAT_451b_1f74)[(iVar5 - DAT_3c04_0177) * 9 + (iVar6 - DAT_3c04_0175)] =
                 *(undefined1 *)(iVar4 + 0x2d56);
            DAT_451b_1f72 = DAT_451b_1f72 + 1;
          }
          else {
            ((undefined1 *)&DAT_451b_1f74)[(iVar5 - DAT_3c04_0177) * 9 + (iVar6 - DAT_3c04_0175)] =
                 0xff;
          }
        }
      }
    }
    else {
      for (; iVar6 = iVar3, iVar5 < iVar1 + 1; iVar5 = iVar5 + 1) {
        for (; iVar6 < iVar2 + 1; iVar6 = iVar6 + 1) {
          if (*(char *)(iVar6 * 0x70 + iVar5) < '\0') {
            ((undefined1 *)&DAT_451b_1f74)[(iVar5 - DAT_3c04_0177) * 9 + (iVar6 - DAT_3c04_0175)] =
                 0xff;
          }
          else {
            iVar4 = FUN_8611_0f6f(iVar5,iVar6);
            ((undefined1 *)&DAT_451b_1f74)[(iVar5 - DAT_3c04_0177) * 9 + (iVar6 - DAT_3c04_0175)] =
                 *(undefined1 *)(iVar4 + 0x2d56);
            DAT_451b_1f72 = DAT_451b_1f72 + 1;
          }
        }
      }
    }
  }
  return;
}


