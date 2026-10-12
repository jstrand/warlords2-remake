
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_8611_0723(uint param_1)

{
  if (DAT_4125_2a86 == 0) {
    return;
  }
  if (DAT_4125_166a != 0) {
    return;
  }
  if (_LAB_4125_1174 != 0) {
    return;
  }
  if (_LAB_4125_1172 != 0) {
    return;
  }
  if (_LAB_4125_1176 != 0) {
    return;
  }
  if (7 < param_1) goto LAB_8611_07c5;
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)(param_1 * 2 + 0x7e4)) {
  case 0x86877:
    DAT_3c04_017f = DAT_3c04_017f + -1;
    goto LAB_8611_07c5;
  case 0x86883:
    DAT_3c04_017f = DAT_3c04_017f + 1;
    goto LAB_8611_07c5;
  case 0x8688f:
    break;
  case 0x86891:
    goto LAB_8611_07bb;
  case 0x86893:
    DAT_3c04_017f = DAT_3c04_017f + -1;
    break;
  case 0x8689f:
    DAT_3c04_017f = DAT_3c04_017f + -1;
    goto LAB_8611_07bb;
  case 0x868ab:
    DAT_3c04_017f = DAT_3c04_017f + 1;
    break;
  case 0x868c1:
    DAT_3c04_017f = DAT_3c04_017f + 1;
LAB_8611_07bb:
    DAT_3c04_017d = DAT_3c04_017d + 1;
    goto LAB_8611_07c5;
  }
  DAT_3c04_017d = DAT_3c04_017d + -1;
LAB_8611_07c5:
  FUN_8611_0629(DAT_3c04_017d,DAT_3c04_017f);
  FUN_8065_0174();
  return;
}


