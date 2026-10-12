
void __cdecl16far FUN_484e_0a69(byte param_1,int param_2)

{
  byte *pbVar1;
  
  if ((*(int *)(current_player * 2 + 0xd0) == 0) && (current_player <= param_2)) {
    param_2 = param_2 + 1;
  }
  param_2 = current_player * 8 + param_2;
  pbVar1 = (byte *)(param_2 + 0x153b);
  *pbVar1 = *pbVar1 & 0xf3;
  pbVar1 = (byte *)(param_2 + 0x153b);
  *pbVar1 = *pbVar1 | (param_1 & 3) << 2;
  if (*(int *)(current_player * 2 + 0xd0) == 0) {
    FUN_22bf_07a5();
    auto_ui_diplomacy_state(1);
    FUN_484e_09d4();
    FUN_22bf_0757();
  }
  return;
}


