
undefined2 __cdecl16far
movetext(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6)

{
  int iVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int local_6;
  int local_4;
  
  iVar1 = __window_check(param_4,param_3,param_2,param_1);
  if ((iVar1 == 0) ||
     (iVar1 = __window_check(param_6 + (param_4 - param_2),param_5 + (param_3 - param_1),param_6,
                             param_5), iVar1 == 0)) {
    uVar2 = 0;
  }
  else {
    local_4 = param_4;
    local_6 = 1;
    iVar1 = param_2;
    if (param_2 < param_6) {
      local_4 = param_2;
      local_6 = -1;
      iVar1 = param_4;
    }
    for (; local_4 + local_6 != iVar1; iVar1 = iVar1 + local_6) {
      uVar3 = __screen_offset(param_6 + (iVar1 - param_2),param_5);
      uVar4 = __screen_offset(iVar1,param_1);
      __text_write((param_3 - param_1) + 1,uVar4,uVar3);
    }
    uVar2 = 1;
  }
  return uVar2;
}


