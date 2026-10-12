
void __cdecl16far
FUN_5c52_0000(int param_1,undefined2 param_2,char *param_3,undefined2 param_4,undefined2 param_5,
             uint param_6)

{
  uint uVar1;
  
  for (uVar1 = 0; uVar1 < param_6; uVar1 = uVar1 + 1) {
    if (*param_3 == '\0') {
      FUN_5c52_0144(param_1 + uVar1,param_2,0x20,param_4,param_5);
    }
    else {
      FUN_5c52_0144(param_1 + uVar1,param_2,(int)*param_3,param_4,param_5);
      param_3 = (char *)CONCAT22(param_3._2_2_,(char *)param_3 + 1);
    }
  }
  return;
}


