
void __cdecl16far FUN_89e0_1e3b(void)

{
  undefined2 uVar1;
  undefined2 in_DX;
  int iVar2;
  
  DAT_451b_2878 = 0;
  uVar1 = FUN_1d28_01d5(0x3a);
  DAT_451b_287a = CONCAT22(in_DX,uVar1);
  if (DAT_451b_287a != 0) {
    for (iVar2 = 0; iVar2 < 0x1d; iVar2 = iVar2 + 1) {
      *(int *)((int)DAT_451b_287a + *(char *)(current_player * 0x1d + iVar2 + 0x60b) * 2) = iVar2;
    }
    FUN_54f6_0000(0);
    DAT_4125_166a = 1;
    auto_str_join_data_not_found(0x22);
    auto_ui_army_bonus(0);
    FUN_89e0_1f51();
  }
  return;
}


