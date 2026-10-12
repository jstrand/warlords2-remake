
void __cdecl16far FUN_6c1b_0e70(void)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_SS;
  char local_18 [20];
  int local_4;
  
  FUN_7563_08c7(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780,0x2e1d,
                ((undefined2 *)&DAT_451b_0c9c)[DAT_451b_04aa]);
  FUN_22bf_07a5();
  auto_ui_army_info_labels_2();
  FUN_78a8_06ae(2,0xf,0,3);
  FUN_54f6_070b(0x58,0x69,0x40,0x28);
  iVar1 = (int)*(char *)(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7778);
  iVar2 = FUN_6c1b_0cad(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780,0x2e1d);
  iVar1 = iVar1 + iVar2;
  if (9 < iVar1) {
    iVar1 = 9;
  }
  local_4 = FUN_6c1b_0c27(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780,0x2e1d);
  local_4 = *(int *)(iVar1 * 2 + 0xce4) + local_4;
  uVar3 = FUN_6c1b_0cad(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780,0x2e1d);
  sprintf((char *)CONCAT22(unaff_SS,local_18),(char *)0x41250dbb,uVar3);
  FUN_7ecb_00bf(0x188,0xa5,local_18);
  sprintf((char *)CONCAT22(unaff_SS,local_18),(char *)0x41250dbf,local_4);
  FUN_7ecb_00bf(0x188,0xb9,local_18);
  FUN_6c1b_0620();
  FUN_22bf_0757();
  return;
}


