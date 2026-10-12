
void __cdecl16far random_map_progress(int param_1)

{
  int iVar1;
  undefined2 unaff_SS;
  undefined1 local_16 [4];
  undefined2 local_12 [5];
  undefined1 local_8 [2];
  undefined2 local_6;
  int local_4;
  
  local_6 = 0x15;
  local_4 = ((param_1 + 10) / 10) * 0x10 + 0x10;
  FUN_216d_002e(local_12);
  FUN_54f6_093a();
  FUN_216d_000e(local_16);
  __F_SPUSH(local_16._0_2_,local_16._2_2_);
  FUN_1997_027e();
  FUN_24d0_0002();
  FUN_54f6_08f6(local_8);
  iVar1 = FUN_54f6_093a();
  if (iVar1 == 0x17) {
    FUN_216d_002e(local_12);
    FUN_54f6_070b(local_12[0]);
    FUN_78a8_06ae(2);
    sprintf((char *)CONCAT22(unaff_SS,&stack0xffe0),(char *)0x4125028a);
  }
  else {
    FUN_216d_002e(local_12);
    FUN_54f6_070b(local_12[0]);
    FUN_78a8_06ae(2);
    sprintf((char *)CONCAT22(unaff_SS,&stack0xffe0),(char *)0x4125028e);
  }
  FUN_7ecb_00d6(0x148);
  return;
}


