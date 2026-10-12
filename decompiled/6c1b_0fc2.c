
void __cdecl16far FUN_6c1b_0fc2(void)

{
  bool bVar1;
  int iVar2;
  
  bVar1 = false;
  iVar2 = 0;
  do {
    if (DAT_451b_08ce <= iVar2) {
LAB_6c1b_0ff1:
      FUN_22bf_07a5();
      if (bVar1) {
        DAT_4125_0d6c = 1;
        if (iVar2 < 1) {
          DAT_451b_04a8 = -1;
        }
        else {
          DAT_451b_04a8 = iVar2 + -1;
        }
        if (iVar2 < DAT_451b_08ce + -1) {
          DAT_451b_04ac = iVar2 + 1;
        }
        else {
          DAT_451b_04ac = -1;
        }
        DAT_451b_04aa = iVar2;
        auto_ui_army_info_labels_2();
      }
      FUN_6c1b_0620();
      FUN_22bf_0757();
      return;
    }
    if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[iVar2] * 0x1d + 0xcff) == '\x01') {
      bVar1 = true;
      goto LAB_6c1b_0ff1;
    }
    iVar2 = iVar2 + 1;
  } while( true );
}


