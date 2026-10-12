
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_save_game(void)

{
  int iVar1;
  char *pcVar2;
  undefined2 uVar3;
  
  DAT_451b_08ce = 10;
  for (iVar1 = 0; iVar1 < 10; iVar1 = iVar1 + 1) {
    ((undefined2 *)&DAT_451b_0c9c)[iVar1] = iVar1;
    strcpy((char *)CONCAT22(0x4125,(undefined1 *)&DAT_451b_08dc + iVar1 * 0x20),
           (char *)CONCAT22(0x4125,(char *)(iVar1 * 0xf + 0x471c)));
  }
  uVar3 = _LAB_4125_128d_1;
  pcVar2 = get_string(0x2e,0);
  FUN_796c_0000(0x8e,0x407f,pcVar2,uVar3);
  return;
}


