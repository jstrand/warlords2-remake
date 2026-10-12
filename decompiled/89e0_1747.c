
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far
auto_str_select_armies_when_present_2(int param_1,undefined2 param_2,undefined2 param_3)

{
  undefined2 uVar1;
  
  if (_LAB_4125_1174 == 0) {
    if (param_1 < DAT_4125_30f6) {
      uVar1 = (undefined2)
              ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + param_1 * 2) >> 0x10);
      ui_army_info((int)*(char *)(((undefined2 *)&DAT_451b_2902)[param_1 * 2] + 4),
                   (int)*(char *)(((undefined2 *)&DAT_451b_2902)[param_1 * 2] + 6),
                   (int)*(char *)(((undefined2 *)&DAT_451b_2902)[param_1 * 2] + 8),
                   (int)*(char *)(((undefined2 *)&DAT_451b_2902)[param_1 * 2] + 0xb),
                   (int)*(char *)(((undefined2 *)&DAT_451b_2902)[param_1 * 2] + 7),
                   (int)*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + param_1 * 2) +
                                 10),param_2,param_3);
    }
    else {
      FUN_740d_1201(param_2,param_3,0);
      FUN_740d_10e1((char *)s_Select_Army_4125_3222,0x4125,
                    (char *)s_Select_armies_when_present_4125_322e,0x4125);
    }
  }
  return;
}


