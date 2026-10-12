
void __cdecl16far FUN_6a35_0094(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  int local_6;
  int local_4;
  
  local_4 = 0;
  local_6 = 0;
  iVar2 = 1;
  for (iVar3 = 0; iVar3 < combat_log_len; iVar3 = iVar3 + 1) {
    iVar1 = kbhit();
    if (iVar1 != 0) {
      iVar1 = getch();
      if (iVar1 == 0) {
        iVar1 = getch();
      }
      if (iVar1 == 0x20) {
        iVar2 = 0;
      }
    }
    if (((undefined1 *)&combat_log)[iVar3] == '\x01') {
      if (iVar2 != 0) {
        sound_casualty(0);
      }
      FUN_6a35_0000(*(undefined2 *)(local_4 * 2 + 0x43f8),*(undefined2 *)(local_4 * 2 + 0x43a8),
                    iVar2);
      local_4 = local_4 + 1;
    }
    else {
      if (iVar2 != 0) {
        sound_casualty(1);
      }
      FUN_6a35_0000(((undefined2 *)&DAT_451b_0458)[local_6],((undefined2 *)&DAT_451b_0408)[local_6],
                    iVar2);
      local_6 = local_6 + 1;
    }
    if (iVar2 == 0) {
      uVar4 = 3;
    }
    else {
      FUN_7ecb_0000(3);
      uVar4 = 5;
    }
    FUN_7ecb_0000(uVar4);
  }
  FUN_7ecb_01f1();
  return;
}


