
void __cdecl16far FUN_57ea_0e4b(int param_1,undefined2 param_2)

{
  int iVar1;
  undefined2 unaff_SS;
  int local_16 [8];
  int local_6;
  int local_4;
  
  iVar1 = 8;
  while (iVar1 != 0) {
    local_16[iVar1 + -1] = -1;
    iVar1 = iVar1 + -1;
  }
  local_16[0] = param_1;
  FUN_623c_0ae7(local_16);
  iVar1 = (int)*(char *)(param_1 * 0x16 + -0x7779) / 5 + 1;
  while( true ) {
    if (*(char *)(param_1 * 0x16 + -0x7779) < '\x02') {
      return;
    }
    local_4 = *(int *)(param_1 * 0x16 + -0x7780);
    local_6 = *(int *)(param_1 * 0x16 + -0x777e);
    FUN_57ea_0f46(param_1,param_2);
    if ((*(int *)(param_1 * 0x16 + -0x7780) == local_4) &&
       (*(int *)(param_1 * 0x16 + -0x777e) == local_6)) {
      return;
    }
    if (*(char *)(param_1 * 0x16 + -0x7779) < '\x04') {
      return;
    }
    if (iVar1 == 0) break;
    if ((*(uint *)(param_1 * 0x16 + -0x7774) & 0x20) == 0) {
      return;
    }
    iVar1 = iVar1 + -1;
  }
  return;
}


