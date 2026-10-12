
undefined2 __cdecl16far auto_str_is_being_razed(int param_1,int param_2)

{
  int *piVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_SS;
  byte abStack_11 [7];
  int local_a;
  uint local_8;
  int local_6;
  
  local_6 = city_sack_value(param_1);
  if (param_2 == 0) {
    local_a = 0;
    FUN_5db9_0656(param_1,0,abStack_11 + 1);
    iVar4 = 6;
    while (iVar3 = iVar4, iVar4 = iVar3 + -1, iVar3 != 0) {
      local_8 = (uint)abStack_11[iVar3];
      if (((local_8 != 0xff) && (*(char *)(local_8 * 0x41 + 0x1592) == current_player)) &&
         ((byte)(&stack0xffe9)[iVar3] < 0x2d)) {
        local_a = local_a + 1;
      }
    }
    if (2 < local_a) {
      return 0xffff;
    }
  }
  if (399 < local_6) {
    auto_str_is_being_sacked();
    return 0xffff;
  }
  sprintf(&DAT_451b_1d28,s__s_is_being_razed__4125_0788,param_1 * 0x41 + 0x1581);
  FUN_67cc_16b3((undefined1 *)&DAT_451b_1d28);
  city_make_ruins(*(int *)(param_1 * 0x41 + 0x157d),*(int *)(param_1 * 0x41 + 0x157f));
  iVar4 = dice(1,0xf,0);
  piVar1 = (int *)(current_player * 2 + 0x10e3);
  *piVar1 = *piVar1 + iVar4 + 10;
  FUN_5ca7_10a1(*(undefined2 *)(param_1 * 0x41 + 0x157d),*(undefined2 *)(param_1 * 0x41 + 0x157f));
  uVar2 = FUN_623c_1851();
  return uVar2;
}


