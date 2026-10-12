
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl16far FUN_6ef3_02fb(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  _LAB_4125_0e3f_1 = 0;
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    if (*(int *)(iVar1 * 2 + 0x137) == 0) {
      ((undefined2 *)&DAT_451b_04e0)[iVar1] = 1;
    }
    else {
      ((undefined2 *)&DAT_451b_04e0)[iVar1] = 0;
    }
    ((undefined2 *)&DAT_451b_04f0)[iVar1] = 0;
  }
  FUN_828e_03f9();
  if (4 < DAT_4125_0df6) {
    return 0;
  }
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)(DAT_4125_0df6 * 2 + 0x5f5)) {
  case 0x6f28b:
    iVar1 = army_count;
    while (iVar2 = iVar1 + -1, iVar1 != 0) {
      iVar1 = iVar2;
      if ((*(char *)(iVar2 * 0x16 + -0x777b) < '\b') && (-1 < *(char *)(iVar2 * 0x16 + -0x777b))) {
        ((undefined2 *)&DAT_451b_04f0)[*(char *)(iVar2 * 0x16 + -0x777b)] =
             ((undefined2 *)&DAT_451b_04f0)[*(char *)(iVar2 * 0x16 + -0x777b)] + 1;
      }
    }
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      if (_LAB_4125_0e3f_1 < (int)((undefined2 *)&DAT_451b_04f0)[iVar1]) {
        _LAB_4125_0e3f_1 = ((undefined2 *)&DAT_451b_04f0)[iVar1];
      }
    }
    if ((_LAB_4125_0e3f_1 == 0) || (_LAB_4125_0e3f_1 / 2 != (_LAB_4125_0e3f_1 + -1) / 2))
    goto LAB_6ef3_03ea;
    break;
  case 0x6f32d:
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      ((undefined2 *)&DAT_451b_04f0)[iVar1] = ((undefined2 *)&side_city_count)[iVar1];
      if (_LAB_4125_0e3f_1 < (int)((undefined2 *)&DAT_451b_04f0)[iVar1]) {
        _LAB_4125_0e3f_1 = ((undefined2 *)&DAT_451b_04f0)[iVar1];
      }
    }
    if ((_LAB_4125_0e3f_1 == 0) || (_LAB_4125_0e3f_1 / 2 != (_LAB_4125_0e3f_1 + -1) / 2))
    goto LAB_6ef3_03ea;
    break;
  case 0x6f383:
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      ((undefined2 *)&DAT_451b_04f0)[iVar1] = *(undefined2 *)(iVar1 * 0x14 + 0x185);
      if (_LAB_4125_0e3f_1 < (int)((undefined2 *)&DAT_451b_04f0)[iVar1]) {
        _LAB_4125_0e3f_1 = ((undefined2 *)&DAT_451b_04f0)[iVar1];
      }
    }
    if ((_LAB_4125_0e3f_1 == 0) || (_LAB_4125_0e3f_1 / 2 != (_LAB_4125_0e3f_1 + -1) / 2))
    goto LAB_6ef3_03ea;
    break;
  case 0x6f3e8:
    iVar1 = FUN_6f8c_07e5();
    return iVar1;
  case 0x6f3f2:
    compute_income();
    compute_upkeep();
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      iVar3 = 0;
      for (iVar2 = 0; iVar2 < city_count; iVar2 = iVar2 + 1) {
        if (*(char *)(iVar2 * 0x41 + 0x1592) == (char)iVar1) {
          iVar3 = iVar3 + (int)*(char *)(iVar2 * 0x41 + 0x1591) * *(int *)(iVar2 * 0x41 + 0x15a7);
        }
      }
      ((undefined2 *)&DAT_451b_04f0)[iVar1] =
           (*(int *)(iVar1 * 0x14 + 0x185) + ((undefined2 *)&side_income)[iVar1] * 5 +
            ((undefined2 *)&side_upkeep)[iVar1] + iVar3) / 0x1e;
      if (500 < (int)((undefined2 *)&DAT_451b_04f0)[iVar1]) {
        ((undefined2 *)&DAT_451b_04f0)[iVar1] = 500;
      }
      if ((int)((undefined2 *)&DAT_451b_04f0)[iVar1] < 1) {
        ((undefined2 *)&DAT_451b_04f0)[iVar1] = 1;
      }
    }
    iVar2 = 0;
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      if ((int)((undefined2 *)&DAT_451b_04f0)[current_player] <
          (int)((undefined2 *)&DAT_451b_04f0)[iVar1]) {
        iVar2 = iVar2 + 1;
      }
    }
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      ((undefined2 *)&DAT_451b_04f0)[iVar1] = (int)((undefined2 *)&DAT_451b_04f0)[iVar1] / 5;
    }
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      if (_LAB_4125_0e3f_1 < (int)((undefined2 *)&DAT_451b_04f0)[iVar1]) {
        _LAB_4125_0e3f_1 = ((undefined2 *)&DAT_451b_04f0)[iVar1];
      }
    }
    _LAB_4125_0e3f_1 = 100;
    return iVar2;
  }
  _LAB_4125_0e3f_1 = _LAB_4125_0e3f_1 + 1;
LAB_6ef3_03ea:
  return ((undefined2 *)&DAT_451b_04f0)[current_player];
}


