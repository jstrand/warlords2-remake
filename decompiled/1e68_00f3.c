
void __cdecl16far
FUN_1e68_00f3(undefined2 param_1,uint *param_2,int param_3,undefined2 param_4,uint *param_5,
             int param_6,int param_7,int param_8,undefined2 param_9,uint *param_10,int param_11)

{
  int iVar1;
  
  iVar1 = param_7;
  do {
    do {
      *param_5 = *param_5 & *param_10 | ~*param_10 & *param_2;
      param_10 = param_10 + 1;
      param_5 = param_5 + 1;
      param_2 = param_2 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
    param_2 = param_2 + (param_3 - param_7);
    param_5 = param_5 + (param_6 - param_7);
    param_10 = param_10 + (param_11 - param_7);
    param_8 = param_8 + -1;
    iVar1 = param_7;
  } while (param_8 != 0);
  return;
}


