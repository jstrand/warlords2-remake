
void __cdecl16far FUN_7bab_0ee8(int param_1)

{
  int iVar1;
  
  if (param_1 == 0) {
    opt_all_computer = 0;
  }
  else {
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      if (*(int *)(iVar1 * 2 + 0x137) != 0) {
        *(undefined2 *)(iVar1 * 2 + 0xd0) = 1;
        if (*(int *)(iVar1 * 2 + 0xc0) != 2) {
          *(undefined2 *)(iVar1 * 2 + 0xe0) = 0;
        }
        *(undefined2 *)(iVar1 * 2 + 0xc0) = 2;
      }
    }
    opt_all_computer = 1;
  }
  FUN_22bf_07a5();
  FUN_7bab_0634(0xffff);
  difficulty_rating();
  FUN_7bab_0416();
  FUN_22bf_0757();
  return;
}


