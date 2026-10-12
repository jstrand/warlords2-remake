
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_str_user_defined_button(int param_1,undefined2 param_2,undefined2 param_3)

{
  if ((param_1 < 1) || (4 < param_1)) {
    if ((param_1 < 5) || (0x18 < param_1)) {
      if ((param_1 < 0x21) || (0x24 < param_1)) {
        if ((param_1 < 0x25) || (0x2c < param_1)) {
          if (param_1 < 0x2d) {
            return;
          }
          if (0x47 < param_1) {
            return;
          }
          FUN_6a89_1475(param_1 + -0x2d,param_2,param_3);
        }
        else {
          auto_str_select_armies_when_present_2(param_1 + -0x25,param_2,param_3);
        }
      }
      else {
        FUN_545c_03f8(param_1 + -0x21);
        FUN_740d_1201(param_2,param_3,0);
        FUN_740d_1158(0x5e6,0x4125,(undefined1 *)&DAT_451b_1cd8,0x4125);
      }
    }
    else {
      FUN_7087_11f4(1,param_1 + -5);
    }
  }
  else {
    if (*(char *)(DAT_4125_0e98 * 0x41 + param_1 + 0x1592) < '\0') {
      return;
    }
    ui_army_type_info((int)*(char *)(DAT_4125_0e98 * 0x41 + param_1 + 0x1592),
                      (int)*(char *)(DAT_4125_0e98 * 0x41 + param_1 + 0x1596),
                      (int)*(char *)(DAT_4125_0e98 * 0x41 + param_1 + 0x159a),
                      (int)*(char *)(DAT_4125_0e98 * 0x41 + param_1 + 0x15a2),
                      (int)*(char *)(DAT_4125_0e98 * 0x41 + param_1 + 0x159e),param_2,param_3);
  }
  _LAB_4125_1174 = 1;
  return;
}


