
void __cdecl16far FUN_6a35_0000(int param_1,int param_2,int param_3)

{
  undefined4 local_e;
  undefined1 local_a [8];
  
  FUN_216d_002e(local_a);
  FUN_216d_000e(&local_e);
  __F_SPUSH(local_e);
  FUN_1997_027e();
  if (param_3 == 0) {
    local_e._2_2_ = 2;
  }
  else {
    local_e._2_2_ = 5;
  }
  local_e = CONCAT22(local_e._2_2_,0x1997);
  FUN_7ecb_0000();
  local_e = CONCAT22(0x11a - (param_2 + -0x3c),0x120 - (param_1 + -0xa0));
  FUN_54f6_07e4(param_1 + -0xa0,param_2 + -0x3c);
  return;
}


