
void __cdecl16far FUN_7bab_1273(int param_1)

{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < 6; iVar1 = iVar1 + 1) {
    ((undefined2 *)&DAT_4125_23b4)[iVar1] = ((undefined2 *)&DAT_4125_2378)[iVar1 + param_1 * 10];
  }
  difficulty_rating();
  FUN_7bab_0416();
  return;
}


