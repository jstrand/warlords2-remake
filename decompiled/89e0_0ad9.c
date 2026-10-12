
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_str_select_armies_when_present(int param_1,undefined2 param_2,uint param_3)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_SS;
  char local_34 [50];
  
  if (_LAB_4125_1174 != 0) {
    return;
  }
  if (DAT_4125_30f8 == 0) {
    iVar1 = (param_1 + -0x10) / 0x28;
    if (param_1 < 0x150) {
      if (iVar1 < DAT_4125_30f6) {
        uVar2 = (undefined2)
                ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar1 * 2) >> 0x10);
        ui_army_info((int)*(char *)(((undefined2 *)&DAT_451b_2902)[iVar1 * 2] + 4),
                     (int)*(char *)(((undefined2 *)&DAT_451b_2902)[iVar1 * 2] + 6),
                     (int)*(char *)(((undefined2 *)&DAT_451b_2902)[iVar1 * 2] + 8),
                     (int)*(char *)(((undefined2 *)&DAT_451b_2902)[iVar1 * 2] + 0xb),
                     (int)*(char *)(((undefined2 *)&DAT_451b_2902)[iVar1 * 2] + 7),
                     (int)*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar1 * 2) +
                                   10),param_1,param_2);
        _LAB_4125_1174 = 1;
        return;
      }
      FUN_740d_1201(param_1,param_2,0);
    }
    else {
      FUN_740d_1201(param_1,param_2,0);
    }
  }
  else {
    FUN_740d_1201(param_1,param_2,0);
    if (3 < param_3) {
      _LAB_4125_1174 = 1;
      return;
    }
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)(param_3 * 2 + 0xc94)) {
    case 0x8a9cc:
      sprintf((char *)CONCAT22(unaff_SS,local_34),s_You_have__d_cities__4125_318d,
              ((undefined2 *)&side_city_count)[current_player]);
      break;
    case 0x8a9f9:
      sprintf((char *)CONCAT22(unaff_SS,local_34),s_You_have__d_gold__4125_31b2,
              *(undefined2 *)(current_player * 0x14 + 0x185));
      break;
    case 0x8aa2c:
      sprintf((char *)CONCAT22(unaff_SS,local_34),s_You_earn__d_gold__4125_31d2,
              ((undefined2 *)&side_income)[current_player]);
      break;
    case 0x8aa58:
      sprintf((char *)CONCAT22(unaff_SS,local_34),s_You_pay__d_gold__4125_31f0,
              ((undefined2 *)&side_upkeep)[current_player]);
    }
  }
  FUN_740d_10e1();
  _LAB_4125_1174 = 1;
  return;
}


