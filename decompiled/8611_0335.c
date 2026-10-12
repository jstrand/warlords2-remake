
void __cdecl16far
FUN_8611_0335(undefined4 param_1,int param_2,int param_3,int param_4,int param_5,int param_6)

{
  undefined4 local_16;
  undefined4 local_12;
  undefined4 local_e;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  if (param_2 == 0xf) {
    param_2 = 8;
  }
  local_4 = param_3 % 0x10 << 5;
  local_6 = (param_3 / 0x10) * 0x1e;
  local_8 = ((undefined2 *)&DAT_4125_2d2a)[param_5];
  local_a = *(int *)(param_6 * 2 + 0x2d3c);
  FUN_1997_034a();
  FUN_1997_060a();
  (*DAT_451b_2a68)(0x1997,param_2 * 0x3b + 0x4f20,0x4125,local_4,local_6,0x1d0020,param_1,
                   local_8 + 8,local_a + 7,0x4ee5,0x4125,local_4,local_6);
  FUN_1997_06bb();
  FUN_2012_0f08(param_1,local_8 + 2,local_a,0xe0028);
  FUN_2012_0f08(param_1,local_8 + 3,local_a,0xd0028);
  FUN_2012_0f08(param_1,local_8 + 4,local_a,0xe0028);
  if (4 < param_4) {
    FUN_1997_060a();
    (*DAT_451b_2a68)(0x1997,param_2 * 0x3b + 0x4f20,0x4125,0x1d0,DAT_4125_2d4e,0x80030,param_1,
                     local_8,local_a + 8,0x4ee5,0x4125,0x1d0,DAT_4125_2d4e);
    param_4 = param_4 + -4;
    FUN_1997_06bb();
  }
  if (3 < param_4) {
    param_4 = 4;
  }
  FUN_1997_060a();
  (*DAT_451b_2a68)(0x1997,param_2 * 0x3b + 0x4f20,0x4125,0x1d0,
                   ((undefined2 *)&DAT_4125_2d4e)[param_4 + -1],0x80030,param_1,local_8,local_a,
                   0x4ee5,0x4125,0x1d0,((undefined2 *)&DAT_4125_2d4e)[param_4 + -1]);
  FUN_1997_06bb();
  if ((DAT_4125_26c6 != 0) && (param_2 < 8)) {
    FUN_1997_034a();
    FUN_216d_000e(&local_16);
    FUN_216d_002e(&local_12);
    (*DAT_451b_2a68)(0x216d,0x57a7,0x4125,local_12,local_e,param_1,local_16,0x576c,0x4125,local_12);
  }
  return;
}


