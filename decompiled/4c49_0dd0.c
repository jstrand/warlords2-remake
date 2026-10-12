
void __cdecl16far FUN_4c49_0dd0(int param_1,int param_2,int param_3,int param_4,undefined4 param_5)

{
  double dVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  
  if (param_3 == param_1) {
    if (param_2 == param_4) {
LAB_4c49_0de6:
      uVar3 = 0;
    }
    else if (param_4 < param_2) {
LAB_4c49_0fe6:
      uVar3 = 0xffff;
    }
    else {
      if (param_4 <= param_2) {
        return;
      }
LAB_4c49_0f40:
      uVar3 = 1;
    }
    uVar2 = 0;
  }
  else {
    if (param_4 == param_2) {
      if (param_1 == param_3) goto LAB_4c49_0de6;
      if (param_3 < param_1) {
LAB_4c49_0e2a:
        uVar3 = 0;
        goto LAB_4c49_0f0c;
      }
      if (param_3 <= param_1) {
        return;
      }
LAB_4c49_0e3a:
      uVar3 = 0;
    }
    else {
      dVar1 = (double)(param_4 - param_2) / (double)(param_3 - param_1);
      if (param_3 < param_1) {
        if (DAT_4125_0292 <= dVar1) goto LAB_4c49_0fe6;
        if ((DAT_4125_0292 <= dVar1) || (dVar1 < DAT_4125_029a)) {
          if ((dVar1 < DAT_4125_029a) && (DAT_4125_02a2 <= dVar1)) goto LAB_4c49_0e2a;
          if ((DAT_4125_02a2 <= dVar1) || (dVar1 < DAT_4125_02aa)) {
            if (DAT_4125_02aa <= dVar1) {
              return;
            }
            goto LAB_4c49_0f40;
          }
          uVar3 = 1;
        }
        else {
          uVar3 = 0xffff;
        }
LAB_4c49_0f0c:
        uVar2 = 0xffff;
        goto LAB_4c49_0dea;
      }
      if (DAT_4125_0292 <= dVar1) goto LAB_4c49_0f40;
      if ((DAT_4125_0292 <= dVar1) || (dVar1 < DAT_4125_029a)) {
        if ((dVar1 < DAT_4125_029a) && (DAT_4125_02a2 <= dVar1)) goto LAB_4c49_0e3a;
        if ((DAT_4125_02a2 <= dVar1) || (dVar1 < DAT_4125_02aa)) {
          if (DAT_4125_02aa <= dVar1) {
            return;
          }
          goto LAB_4c49_0fe6;
        }
        uVar3 = 0xffff;
      }
      else {
        uVar3 = 1;
      }
    }
    uVar2 = 1;
  }
LAB_4c49_0dea:
  FUN_216d_000e(param_5,uVar2,uVar3);
  return;
}


