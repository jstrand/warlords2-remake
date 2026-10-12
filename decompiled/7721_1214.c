
void __cdecl16far auto_ui_save_map(void)

{
  int iVar1;
  char *pcVar2;
  
  if (((DAT_3c04_0130 == 0) && (DAT_3c04_0120 != 0)) && (DAT_4125_28c6 == 0)) {
    DAT_451b_08ce = 10;
    for (iVar1 = 0; iVar1 < 10; iVar1 = iVar1 + 1) {
      ((undefined2 *)&DAT_451b_0c9c)[iVar1] = iVar1;
      strcpy((char *)CONCAT22(0x4125,(undefined1 *)&DAT_451b_08dc + iVar1 * 0x20),
             (char *)CONCAT22(0x4125,(char *)(iVar1 * 0xf + 0x467c)));
    }
    pcVar2 = get_string(0x30,0);
    FUN_796c_0000(0xa2,0x407f,(char *)pcVar2,(int)((ulong)pcVar2 >> 0x10));
  }
  return;
}


