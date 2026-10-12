
void __cdecl16far military_advisor(void)

{
  char *pcVar1;
  int index;
  undefined2 local_e;
  undefined2 in_stack_0000fff4;
  
  if (opt_military_advisor != 0) {
    FUN_22bf_07a5();
    FUN_54f6_0000();
    FUN_78a8_06ae(0xf0001,0x30000);
    pcVar1 = get_string(0x7b,0);
    FUN_7ecb_00d6(0x430140,(char *)pcVar1,(int)((ulong)pcVar1 >> 0x10));
    auto_str_join_data_not_found();
    DAT_4125_166a = 1;
    FUN_216d_002e(&stack0xfff6);
    FUN_216d_000e(&local_e);
    __F_SPUSH(CONCAT22(in_stack_0000fff4,local_e));
    FUN_1997_0129();
    local_e = 0x1997;
    FUN_18a9_066e();
    local_e = 0x18a9;
    advisor_simulate_wins();
    local_e = 0xf;
    FUN_78a8_06ae(2);
    local_e = 0x78a8;
    get_string(0x7c,-1);
    local_e = 0x102;
    FUN_7ecb_00d6(0x140);
    local_e = 0x7ecb;
    get_string(0x7d,-1);
    index = 0x140;
    local_e = 0x116;
    FUN_7ecb_00d6();
    local_e = 0x7ecb;
    get_string(0x7e,index);
    local_e = 0x12a;
    FUN_7ecb_00d6(0x140);
    FUN_22bf_0757();
  }
  return;
}


