
void __cdecl16far auto_ui_sage_greeting(undefined2 param_1)

{
  char *pcVar1;
  undefined2 uVar2;
  
  FUN_22bf_07a5();
  FUN_54f6_0000(2);
  FUN_78a8_06ae(1,0xf,0,3);
  FUN_7ecb_00d6(0x1b0,0x3e,0xc7c,0x4125);
  FUN_78a8_06ae(2,0xf,0,3);
  pcVar1 = get_string(0x3f,0);
  FUN_7ecb_00d6(0x1b0,0x78,pcVar1);
  pcVar1 = get_string(0x3f,1);
  FUN_7ecb_00d6(0x1b0,0x8c,pcVar1);
  pcVar1 = get_string(0x3f,2);
  FUN_7ecb_00d6(0x1b0,0xa0,pcVar1);
  pcVar1 = get_string(0x3f,3);
  FUN_7ecb_00d6(0x1b0,0xb4,pcVar1);
  pcVar1 = get_string(0x3f,4);
  FUN_7ecb_00d6(0x1b0,200,pcVar1);
  DAT_451b_01c6 = 0xf0;
  auto_str_join_data_not_found(10);
  DAT_4125_166a = 1;
  FUN_18a9_03b3(0x11a);
  if (DAT_451b_08ce < 1) {
    uVar2 = 2;
  }
  else {
    uVar2 = 1;
  }
  FUN_18a9_066e(0x117,uVar2);
  FUN_18a9_066e(0x118,1);
  FUN_18a9_0839(0xf);
  if (opt_hidden_map == 0) {
    uVar2 = 2;
  }
  else {
    uVar2 = 1;
  }
  FUN_18a9_066e(0x119,uVar2);
  FUN_834b_05e4(0,0,param_1);
  FUN_834b_04d3(0,0);
  FUN_22bf_0757();
  return;
}


