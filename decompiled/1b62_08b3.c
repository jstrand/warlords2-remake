
void __cdecl16far FUN_1b62_08b3(int param_1,int param_2)

{
  if (DAT_451b_1f02 != 0) {
    DAT_451b_1f02 = 0;
    if (param_2 != 0) {
      FUN_828e_0af3();
    }
    if (param_1 != 0) {
      FUN_8611_006c();
    }
    if (*(int *)(current_player * 2 + 0xd0) == 0) {
      FUN_89e0_0356();
      FUN_8065_0174();
    }
  }
  return;
}


