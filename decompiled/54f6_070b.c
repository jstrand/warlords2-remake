
void __cdecl16far FUN_54f6_070b(int param_1,int param_2,undefined2 param_3,undefined2 param_4)

{
  int iVar1;
  undefined4 local_e;
  int local_a;
  int local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  if (-1 < DAT_4125_0636 + -1) {
    iVar1 = *(int *)((DAT_4125_0636 + -1) * 2 + 0x3fb2);
    FUN_216d_002e(&local_a);
    local_e = *(undefined4 *)(iVar1 * 4 + 0x638);
    if ((iVar1 == 2) || (iVar1 == 10)) {
      local_a = local_a + DAT_4125_2a8c;
      local_e._2_2_ = (int)((ulong)local_e >> 0x10);
      local_e = CONCAT22(local_e._2_2_,(int)local_e + DAT_4125_2a8c);
    }
    local_a = local_a + param_1;
    local_6 = param_3;
    local_8 = local_8 + param_2;
    local_4 = param_4;
    local_e = CONCAT22(local_e._2_2_ + param_2,(int)local_e + param_1);
    __F_SPUSH(local_e);
    FUN_1997_0129();
  }
  return;
}


