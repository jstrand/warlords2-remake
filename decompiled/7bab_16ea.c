
void __cdecl16far FUN_7bab_16ea(int param_1)

{
  int iVar1;
  int iVar2;
  
  if (*(int *)(param_1 * 2 + 0x137) == 0) {
    FUN_7bab_0416();
  }
  else {
    DAT_451b_19f6 = *(undefined2 *)(param_1 * 2 + 0xe0);
    DAT_4125_2563 = param_1;
    strcpy((char *)0x41255964,(char *)CONCAT22(0x3c04,(char *)(param_1 * 0x14)));
    for (iVar2 = 0; iVar2 < 5; iVar2 = iVar2 + 1) {
      ((undefined2 *)&DAT_451b_19fa)[iVar2] = 0xffff;
    }
    if (*(int *)(param_1 * 2 + 0xd0) == 1) {
      list_carried_items(7,*(undefined2 *)(param_1 * 2 + 0xc0),0xffff,0);
      for (iVar2 = 0; iVar2 < 5; iVar2 = iVar2 + 1) {
        iVar1 = iVar2;
        if (DAT_451b_08ce <= iVar2) {
          iVar1 = -1;
        }
        ((undefined2 *)&DAT_451b_19fa)[iVar2] = iVar1;
      }
      DAT_451b_19f8 = *(undefined2 *)(DAT_4125_2563 * 2 + 0xe0);
      if (4 < *(int *)(DAT_4125_2563 * 2 + 0xe0)) {
        for (iVar2 = 0; iVar2 < 5; iVar2 = iVar2 + 1) {
          ((undefined2 *)&DAT_451b_19fa)[iVar2] =
               ((undefined2 *)&DAT_451b_19fa)[iVar2] + *(int *)(DAT_4125_2563 * 2 + 0xe0) + -4;
        }
      }
    }
    FUN_54f6_0000();
    auto_str_setup_side();
    DAT_4125_166a = 1;
    auto_str_join_data_not_found();
    FUN_7bab_1ae4();
  }
  return;
}


