
void __cdecl16far
FUN_7b4c_0000(undefined4 param_1,undefined4 param_2,undefined4 param_3,char *param_4,
             undefined4 param_5,undefined4 param_6,undefined2 param_7,undefined2 param_8,int param_9
             )

{
  DAT_4125_166a = 1;
  DAT_451b_1970 = 0;
  DAT_451b_196e = param_7;
  DAT_451b_1974 = param_8;
  if (param_9 < 1) {
    param_9 = 0xe8;
  }
  DAT_451b_19cc = param_5;
  DAT_451b_19c8 = param_6;
  DAT_451b_1972 = param_9;
  strcpy((char *)0x412558d8,param_4);
  DAT_451b_1976 = strlen((char *)0x412558d8);
  FUN_22bf_07a5();
  FUN_7b4c_00e2(param_1,param_2,param_3);
  auto_str_join_data_not_found();
  FUN_7b4c_02c7();
  FUN_22bf_0757();
  return;
}


