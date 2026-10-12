
undefined2 __cdecl16far FUN_828e_0b51(int param_1,int param_2,int param_3,int param_4)

{
  undefined2 uVar1;
  
  if (param_1 == param_3) {
    if (param_2 < param_4) {
      uVar1 = 4;
    }
    else {
      uVar1 = 0;
    }
  }
  else if (param_2 == param_4) {
    if (param_1 < param_3) {
      uVar1 = 2;
    }
    else {
      uVar1 = 6;
    }
  }
  else if ((param_3 < param_1) && (param_4 < param_2)) {
    uVar1 = 7;
  }
  else if ((param_3 < param_1) && (param_2 < param_4)) {
    uVar1 = 5;
  }
  else if ((param_1 < param_3) && (param_4 < param_2)) {
    uVar1 = 1;
  }
  else if ((param_1 < param_3) && (param_2 < param_4)) {
    uVar1 = 3;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}


