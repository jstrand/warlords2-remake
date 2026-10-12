
void __cdecl16far
FUN_8611_0bf7(int param_1,int param_2,undefined4 param_3,undefined2 param_4,undefined2 param_5)

{
  if (param_1 == 0xf) {
    param_1 = 8;
  }
  FUN_1997_034a(5);
  FUN_1997_034a(0x26);
  if (((param_2 == 0) || (param_2 == 1)) || (param_2 == 2)) {
    param_1 = param_1 * 0x28;
  }
  else {
    if (param_2 != 3) {
      if (param_2 != 4) {
        return;
      }
      (*DAT_451b_2a68)(0x1997,0x53bc,0x4125,param_1 << 5,0x24,0x170020,param_3,param_4,param_5,
                       0x5381,0x4125,param_1 << 5,0x24);
      return;
    }
    param_1 = param_1 * 0x28 + 0x18;
  }
  (*DAT_451b_2a74)(0x1997,0x55cf,0x4125,param_1);
  return;
}


