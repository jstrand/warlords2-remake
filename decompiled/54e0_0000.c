
void __cdecl16far
FUN_54e0_0000(undefined2 param_1,undefined2 param_2,int param_3,int param_4,int param_5,int param_6)

{
  undefined2 unaff_SS;
  undefined1 local_a [4];
  int local_6;
  int local_4;
  
  FUN_22bf_07a5();
  if (-1 < param_5) {
    local_4 = param_3;
    local_6 = param_4 + 10;
    FUN_216d_000e(local_a);
    __F_SPUSH(local_a._0_2_,local_a._2_2_);
    FUN_1997_0129(0x1e);
  }
  if (param_6 != 0) {
    FUN_78a8_06ae(2,0xf);
    FUN_7087_14f4();
    FUN_7ecb_00bf(param_3 + DAT_4125_060a,param_4 + 10);
    sprintf((char *)CONCAT22(unaff_SS,&stack0xffa6),s_Time___d_4125_060e);
    FUN_7ecb_00bf(param_3 + DAT_4125_060a,param_4 + 0x28);
    sprintf((char *)CONCAT22(unaff_SS,&stack0xffa6),s_Cost___d_4125_0617);
    FUN_7ecb_00bf(param_3 + DAT_4125_060a,param_4 + 0x3c);
    sprintf((char *)CONCAT22(unaff_SS,&stack0xffa6),s_Strength___d_4125_0620);
    FUN_7ecb_00bf(param_3 + DAT_4125_060a,param_4 + 0x50);
    sprintf((char *)CONCAT22(unaff_SS,&stack0xffa6),s_Move___d_4125_062d);
    FUN_7ecb_00bf(param_3 + DAT_4125_060a,param_4 + 100);
  }
  FUN_22bf_0757();
  return;
}


