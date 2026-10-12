
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_load_map(void)

{
  int iVar1;
  char *pcVar2;
  undefined2 uVar3;
  
  DAT_451b_08ce = 0;
  for (iVar1 = 0; iVar1 < 10; iVar1 = iVar1 + 1) {
    if (((undefined1 *)&DAT_451b_0712)[iVar1] != '\0') {
      ((undefined2 *)&DAT_451b_0c9c)[DAT_451b_08ce] = iVar1;
      strcpy((char *)CONCAT22(0x4125,(undefined1 *)&DAT_451b_08dc + DAT_451b_08ce * 0x20),
             (char *)CONCAT22(0x4125,(char *)(iVar1 * 0xf + 0x467c)));
      DAT_451b_08ce = DAT_451b_08ce + 1;
    }
  }
  uVar3 = _LAB_4125_128f_1;
  pcVar2 = get_string(0x31,0);
  FUN_796c_0000(0x9d,0x407f,pcVar2,uVar3);
  return;
}


