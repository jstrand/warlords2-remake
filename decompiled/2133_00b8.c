
void __cdecl16far
FUN_2133_00b8(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6,long param_7)

{
  undefined1 uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined2 uVar10;
  int local_12;
  int local_10;
  int local_e;
  int local_8;
  
  uVar10 = 0x2133;
  local_12 = 0;
  if ((param_3 != param_5) || (param_4 != param_6)) {
    if (param_6 < param_4) {
      FUN_2133_038a(&param_4);
      FUN_2133_038a(&param_3);
    }
    if (param_5 < param_3) {
      FUN_2133_038a(&param_3);
      FUN_2133_038a(&param_4);
    }
    if ((param_2 == 0x4125) && (param_1 == 0x342a)) {
      uVar10 = 0x22bf;
      FUN_22bf_07a5();
    }
    uVar2 = param_5 - param_3;
    uVar3 = param_6 - param_4;
    if ((int)uVar2 < 1) {
      if (uVar2 == 0) {
        iVar4 = 0;
      }
      else {
        iVar4 = -1;
      }
    }
    else {
      iVar4 = 1;
    }
    if ((int)uVar3 < 1) {
      if (uVar3 == 0) {
        iVar5 = 0;
      }
      else {
        iVar5 = -1;
      }
    }
    else {
      iVar5 = 1;
    }
    iVar6 = (uVar2 ^ (int)uVar2 >> 0xf) - ((int)uVar2 >> 0xf);
    iVar7 = (uVar3 ^ (int)uVar3 >> 0xf) - ((int)uVar3 >> 0xf);
    local_10 = 0;
    local_e = 0;
    if (iVar6 < iVar7) {
      iVar8 = param_3;
      iVar9 = param_4;
      for (local_8 = 0; local_8 <= iVar7; local_8 = local_8 + 1) {
        local_e = local_e + iVar6;
        if (iVar7 <= local_e) {
          local_e = local_e - iVar7;
          iVar8 = iVar8 + iVar4;
        }
        if (*(int *)((int)DAT_451b_30e9 + 0x5d) == -1) {
          if (param_7 != 0) {
            (*DAT_451b_2a70)(uVar10,param_1,param_2,iVar8,iVar9,
                             (int)*(char *)((int)param_7 + local_12));
            local_12 = local_12 + 1;
          }
        }
        else {
          if (param_7 != 0) {
            uVar1 = (*DAT_451b_2a6c)(uVar10,param_1,param_2,iVar8,iVar9);
            *(undefined1 *)((int)param_7 + local_12) = uVar1;
            local_12 = local_12 + 1;
          }
          (*DAT_451b_2a70)(uVar10,param_1,param_2,iVar8,iVar9,
                           *(undefined2 *)((int)DAT_451b_30e9 + 0x5d));
        }
        iVar9 = iVar9 + iVar5;
      }
    }
    else {
      iVar8 = param_3;
      iVar9 = param_4;
      for (local_8 = 0; local_8 <= iVar6; local_8 = local_8 + 1) {
        local_10 = local_10 + iVar7;
        if (iVar6 <= local_10) {
          local_10 = local_10 - iVar6;
          iVar9 = iVar9 + iVar5;
        }
        if (*(int *)((int)DAT_451b_30e9 + 0x5d) == -1) {
          if (param_7 != 0) {
            (*DAT_451b_2a70)(uVar10,param_1,param_2,iVar8,iVar9,
                             (int)*(char *)((int)param_7 + local_12));
            local_12 = local_12 + 1;
          }
        }
        else {
          if (param_7 != 0) {
            uVar1 = (*DAT_451b_2a6c)(uVar10,param_1,param_2,iVar8,iVar9);
            *(undefined1 *)((int)param_7 + local_12) = uVar1;
            local_12 = local_12 + 1;
          }
          (*DAT_451b_2a70)(uVar10,param_1,param_2,iVar8,iVar9,
                           *(undefined2 *)((int)DAT_451b_30e9 + 0x5d));
        }
        iVar8 = iVar8 + iVar4;
      }
    }
    if ((param_2 == 0x4125) && (param_1 == 0x342a)) {
      FUN_22bf_0757();
    }
  }
  return;
}


