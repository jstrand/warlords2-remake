
undefined2 __cdecl16far
puttext(int param_1,int param_2,int param_3,int param_4,int param_5,undefined2 param_6)

{
  int iVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  
  iVar1 = __window_check(param_4,param_3,param_2,param_1);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    iVar1 = (param_3 - param_1) + 1;
    for (; param_2 <= param_4; param_2 = param_2 + 1) {
      uVar3 = __screen_offset(param_2,param_1);
      __text_write(iVar1,param_5,param_6,uVar3);
      param_5 = param_5 + iVar1 * 2;
    }
    uVar2 = 1;
  }
  return uVar2;
}


