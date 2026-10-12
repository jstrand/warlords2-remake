
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_6d51_18da(int param_1)

{
  int iVar1;
  undefined2 unaff_SS;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  if (param_1 != 0) {
    __F_SCOPY(0xdce,0x4125,&local_a,unaff_SS);
    FUN_24d0_02e5(0xdce,0x4125,0x20004);
    local_a = local_a + 1;
    local_8 = local_8 + 1;
    local_6 = local_6 + -2;
    local_4 = local_4 + -2;
    FUN_24d0_02e5(&local_a);
    local_a = local_a + 1;
    local_8 = local_8 + 1;
    local_6 = local_6 + -2;
    local_4 = local_4 + -2;
    FUN_24d0_0002();
    FUN_216d_01fd(&local_a);
  }
  iVar1 = (int)(((long)DAT_451b_04de * (long)DAT_4125_0dda) / (long)(DAT_451b_04dc + -1));
  if (DAT_4125_0dda < iVar1) {
    iVar1 = DAT_4125_0dda;
  }
  FUN_216d_002e(&local_a);
  FUN_24d0_0002();
  FUN_216d_0288(&local_a);
  if (iVar1 < DAT_4125_0dda + -8) {
    FUN_216d_002e(&local_a);
    FUN_24d0_0002();
    FUN_216d_0288(&local_a);
  }
  return;
}


