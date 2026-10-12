
void __cdecl16far compute_income(void)

{
  int iVar1;
  int iVar2;
  
  for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
    ((undefined2 *)&side_income)[iVar2] = 0;
  }
  for (iVar2 = 0; iVar2 < city_count; iVar2 = iVar2 + 1) {
    if (*(char *)(iVar2 * 0x41 + 0x1592) < '\b') {
      ((undefined2 *)&side_income)[*(char *)(iVar2 * 0x41 + 0x1592)] =
           ((undefined2 *)&side_income)[*(char *)(iVar2 * 0x41 + 0x1592)] +
           *(int *)(iVar2 * 0x41 + 0x15a7);
    }
  }
  for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
    iVar1 = side_gold_per_city(iVar2);
    ((undefined2 *)&side_income)[iVar2] =
         ((undefined2 *)&side_income)[iVar2] + ((undefined2 *)&side_city_count)[iVar2] * iVar1;
  }
  return;
}


