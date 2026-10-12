
void __cdecl16far FUN_6c1b_115e(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 1000;
  for (iVar2 = 0; iVar2 < DAT_4125_0d68; iVar2 = iVar2 + 1) {
    iVar1 = map_distance(param_1,param_2,
                         *(int *)(((undefined2 *)&DAT_451b_04ae)[iVar2] * 0x16 + -0x7780),
                         *(int *)(((undefined2 *)&DAT_451b_04ae)[iVar2] * 0x16 + -0x777e));
    if (iVar1 < iVar3) {
      iVar3 = iVar1;
      DAT_4125_0d6a = iVar2;
    }
  }
  DAT_4125_0d6c = 0;
  FUN_22bf_07a5();
  auto_ui_army_info_labels();
  FUN_6c1b_10ac();
  FUN_6c1b_0620();
  FUN_22bf_0757();
  return;
}


