
void __cdecl16far apply_income(void)

{
  int *piVar1;
  
  compute_income();
  compute_upkeep();
  piVar1 = (int *)(current_player * 0x14 + 0x185);
  *piVar1 = *piVar1 + (((undefined2 *)&side_income)[current_player] -
                      ((undefined2 *)&side_upkeep)[current_player]);
  if (*(int *)(current_player * 0x14 + 0x185) < 0) {
    *(undefined2 *)(current_player * 0x14 + 0x185) = 0;
  }
  return;
}


