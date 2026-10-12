
void __cdecl16far
FUN_2012_0261(undefined4 param_1,int param_2,int param_3,int param_4,int param_5,undefined4 param_6,
             int param_7,int param_8)

{
  undefined2 uVar1;
  undefined2 local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  while (param_5 != 0) {
    local_4 = param_2;
    local_6 = param_7;
    local_8 = param_4;
    while (local_8 != 0) {
      uVar1 = (*DAT_451b_2a6c)(0x2012,(int)param_1,(int)((ulong)param_1 >> 0x10),local_4,param_3);
      (*DAT_451b_2a70)(0x2012,(int)param_6,(int)((ulong)param_6 >> 0x10),local_6,param_8,uVar1);
      local_8 = local_8 + -1;
      local_6 = local_6 + 1;
      local_4 = local_4 + 1;
    }
    param_3 = param_3 + 1;
    param_8 = param_8 + 1;
    param_5 = param_5 + -1;
  }
  return;
}


