
void __cdecl16far
__matherr_raise(int param_1,char *param_2,undefined2 param_3,undefined8 *param_4,undefined8 *param_5
               ,undefined8 param_6)

{
  int iVar1;
  int local_20;
  char *local_1e;
  undefined2 local_1c;
  undefined8 local_1a;
  undefined8 local_12;
  undefined8 local_a;
  
  local_20 = param_1;
  local_1c = param_3;
  local_1e = param_2;
  if ((undefined8 *)param_4 == (undefined8 *)0x0 && param_4._2_2_ == 0) {
    local_1a = 0;
  }
  else {
    local_1a = *param_4;
  }
  if ((undefined8 *)param_5 == (undefined8 *)0x0 && param_5._2_2_ == 0) {
    local_12 = 0;
  }
  else {
    local_12 = *param_5;
  }
  local_a = param_6;
  iVar1 = matherr((exception *)&local_20);
  if (iVar1 == 0) {
    fprintf((FILE *)0x3b94,(char *)((int)(undefined2 *)&DAT_451b_01c4 + 1),
            (char *)s__s___s_error_4125_3a4e,0x4125,param_2,param_3,
            *(undefined2 *)(param_1 * 4 + 0x38c6),*(undefined2 *)(param_1 * 4 + 0x38c8));
    if ((param_1 == 3) || (param_1 == 4)) {
      DAT_4125_007f = 0x22;
    }
    else {
      DAT_4125_007f = 0x21;
    }
  }
  return;
}


