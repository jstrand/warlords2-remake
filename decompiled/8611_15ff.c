
/* WARNING: Type propagation algorithm not settling */

uint __cdecl16far FUN_8611_15ff(uint param_1,int param_2,int param_3)

{
  char cVar1;
  
  if (opt_hidden_map == 0) {
    return 1;
  }
  if (3 < param_1) {
    return 0;
  }
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)((char *)s_Button_file_not_found_4125_1744 + 0x10 + param_1 * 2)
        ) {
  case 0x8773e:
    cVar1 = ((undefined1 *)&DAT_451b_1f74)[param_2 * 9 + param_3];
    break;
  case 0x87758:
    if (((param_2 < DAT_3c04_0177) || (DAT_3c04_017b < param_2)) ||
       ((param_3 < DAT_3c04_0175 || (DAT_3c04_0179 < param_3)))) {
LAB_8611_1736:
      return (*(byte *)(param_3 * 0x70 + param_2) & 0x20) >> 5;
    }
    cVar1 = ((undefined1 *)&DAT_451b_1f74)
            [(param_2 - DAT_3c04_0177) * 9 + (param_3 - DAT_3c04_0175)];
    break;
  case 0x877b6:
    cVar1 = ((undefined1 *)&DAT_451b_1f74)
            [((param_2 + -0x10) / 0x28) * 9 + (param_3 + -0x1e) / 0x28];
    break;
  case 0x877e0:
    param_2 = (param_2 + -400) / 2;
    param_3 = (param_3 + -0x1e) / 2;
    if ((((param_2 < DAT_3c04_0177) || (DAT_3c04_017b < param_2)) || (param_3 < DAT_3c04_0175)) ||
       (DAT_3c04_0179 < param_3)) goto LAB_8611_1736;
    cVar1 = ((undefined1 *)&DAT_451b_1f74)
            [(param_2 - DAT_3c04_0177) * 9 + (param_3 - DAT_3c04_0175)];
  }
  return (uint)(cVar1 != '\x0e');
}


