
void __cdecl16far FUN_7f77_058d(void)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_SS;
  char local_42 [64];
  
  iVar3 = 0;
  bVar1 = false;
  while (!bVar1) {
    iVar2 = auto_file_scenario_dat();
    if (iVar2 == 0) {
      bVar1 = true;
      DAT_451b_08ce = iVar3;
    }
    else {
      ((undefined2 *)&DAT_451b_0c9c)[iVar3] = iVar3;
      strcpy((char *)CONCAT22(0x4125,(undefined1 *)&DAT_451b_08dc + iVar3 * 0x20),
             (char *)CONCAT22(unaff_SS,local_42));
      iVar3 = iVar3 + 1;
    }
  }
  FUN_7f77_0725();
  return;
}


