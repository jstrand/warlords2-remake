
void __cdecl16far FUN_796c_0000(undefined4 param_1,char *param_2,int param_3)

{
  int iVar1;
  
  strcpy((char *)0x41254816,param_2);
  DAT_451b_08da = param_3;
  DAT_451b_08ca = param_1;
  DAT_451b_08d0 = 0;
  if (DAT_451b_08ce < 2) {
    DAT_451b_08d2 = 0xffff;
  }
  else {
    DAT_451b_08d2 = 1;
  }
  if (DAT_451b_08ce < 3) {
    DAT_451b_08d4 = 0xffff;
  }
  else {
    DAT_451b_08d4 = 2;
  }
  if (DAT_451b_08ce < 4) {
    DAT_451b_08d6 = 0xffff;
  }
  else {
    DAT_451b_08d6 = 3;
  }
  if (DAT_451b_08ce < 5) {
    DAT_451b_08d8 = 0xffff;
  }
  else {
    DAT_451b_08d8 = 4;
  }
  while (4 < DAT_451b_08da) {
    DAT_451b_08da = DAT_451b_08da + -1;
    for (iVar1 = 0; iVar1 < 5; iVar1 = iVar1 + 1) {
      ((undefined2 *)&DAT_451b_08d0)[iVar1] = ((undefined2 *)&DAT_451b_08d0)[iVar1] + 1;
    }
  }
  FUN_22bf_07a5();
  FUN_796c_00b2();
  DAT_4125_166a = 1;
  auto_str_join_data_not_found();
  FUN_796c_0301();
  FUN_22bf_0757();
  return;
}


