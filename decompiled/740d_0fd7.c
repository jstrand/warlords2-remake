
void __cdecl16far auto_str_blessings_quests(int param_1)

{
  int iVar1;
  undefined1 *puVar2;
  
  FUN_78a8_06ae(2,*(undefined2 *)(current_player * 2 + 0xa0),
                *(undefined2 *)(current_player * 2 + 0xb0),3);
  FUN_7ecb_00d6(DAT_451b_0528 + (int)DAT_451b_052c / 2 + -8,DAT_451b_052a + 0xb,
                param_1 * 0x1f + 0x815,0x3c04);
  FUN_78a8_06ae(0xf0002,0x30000);
  if (*(char *)(param_1 * 0x1f + 0x829) == '\x01') {
    puVar2 = &LAB_4125_10e5;
  }
  else {
    if ((*(byte *)(*(int *)(param_1 * 0x1f + 0x813) * 0xe0 + *(int *)(param_1 * 0x1f + 0x811) * 2 +
                  1) & 0x40) != 0) {
      puVar2 = (undefined1 *)0x41251105;
      iVar1 = DAT_451b_052a + 0x21;
      goto LAB_740d_1074;
    }
    puVar2 = (undefined1 *)0x412510f9;
  }
  iVar1 = DAT_451b_052a + 0x23;
LAB_740d_1074:
  FUN_7ecb_00d6(DAT_451b_0528 + (int)DAT_451b_052c / 2 + -8,iVar1,puVar2);
  return;
}


