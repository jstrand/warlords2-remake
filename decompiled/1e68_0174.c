
void __cdecl16far
FUN_1e68_0174(undefined2 param_1,uint *param_2,int param_3,uint *param_4,int param_5,int param_6,
             int param_7,uint *param_8,int param_9)

{
  int iVar1;
  
  iVar1 = param_6;
  do {
    do {
      *param_4 = *param_4 & *param_8 | ~*param_8 & *param_2;
      param_8 = param_8 + 1;
      param_4 = param_4 + 1;
      param_2 = param_2 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
    param_2 = param_2 + (param_3 - param_6);
    param_4 = param_4 + (param_5 - param_6);
    param_8 = param_8 + (param_9 - param_6);
    param_7 = param_7 + -1;
    iVar1 = param_6;
  } while (param_7 != 0);
  return;
}


