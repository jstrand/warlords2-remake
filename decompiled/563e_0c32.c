
undefined2 __cdecl16far
FUN_563e_0c32(undefined2 param_1,int *param_2,uint param_3,int param_4,undefined4 param_5)

{
  byte *pbVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 local_1a;
  int local_18;
  int local_16;
  int local_14;
  int local_12;
  int local_10;
  int local_e;
  uint local_c;
  undefined2 local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_4 = *param_2;
  local_6 = *(int *)(local_4 * 0x16 + -0x7780);
  local_8 = *(int *)(local_4 * 0x16 + -0x777e);
  local_14 = -1;
  local_16 = -1;
  local_18 = 0;
  local_1a = 0;
  if ((*(uint *)(local_4 * 0x16 + -0x7774) & 0x1000) == 0) {
    local_a = 0;
    for (iVar7 = local_6 - param_4; iVar7 < local_6 + param_4; iVar7 = iVar7 + 1) {
      for (iVar8 = local_8 - param_4; iVar8 < local_8 + param_4; iVar8 = iVar8 + 1) {
        if ((((((-1 < iVar7) && (-1 < iVar8)) && (iVar7 < 0x70)) &&
             (((iVar8 < 0x9c && ((*(byte *)(iVar8 * 0xe0 + iVar7 * 2 + 1) & 0x10) != 0)) &&
              ((local_c = *(byte *)(iVar8 * 0xe0 + iVar7 * 2 + 1) & 0xf, local_c == param_3 &&
               ((cVar2 = *(char *)(*(byte *)(iVar8 * 0xe0 + iVar7 * 2) + 0x710), cVar2 != '\n' &&
                (cVar2 != '\x03')))))))) && (cVar2 != '\x02')) &&
           ((uVar4 = *(uint *)((int)param_5 + iVar7 * 0x138 + iVar8 * 2), uVar5 = (int)uVar4 >> 0xf,
            (int)((uVar4 ^ uVar5) - uVar5) <= DAT_451b_1ea0 + -1 &&
            ((*(byte *)(iVar8 * 0x70 + iVar7) & 0x20) != 0)))) {
          FUN_623c_1626(iVar7,iVar8,&local_e);
          iVar3 = FUN_623c_15c5(iVar7,iVar8);
          if (((0x4b < iVar3) && (((2 < local_e && (10 < local_10)) || (local_12 != 0)))) &&
             (local_18 < iVar3)) {
            local_18 = iVar3;
            local_16 = iVar8;
            local_14 = iVar7;
          }
        }
      }
    }
    if (local_14 != -1) {
      if ((*(byte *)(local_4 * 0x16 + -0x7772) & 0xf) == 1) {
        uVar4 = (uint)(*(byte *)(local_4 * 0x16 + -0x7771) >> 1);
      }
      else {
        uVar4 = 0xffff;
      }
      FUN_623c_0ae7(param_2,4,0x200000);
      uVar9 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
      iVar7 = (int)DAT_451b_1f02;
      *(int *)(iVar7 + 0x12) = local_14;
      *(int *)(iVar7 + 0x14) = local_16;
      move_stack_to();
      for (iVar7 = 0; iVar7 < 8; iVar7 = iVar7 + 1) {
        uVar9 = (undefined2)((ulong)param_2 >> 0x10);
        piVar6 = (int *)param_2;
        if (piVar6[iVar7] != -1) {
          if (*(char *)(piVar6[iVar7] * 0x16 + -0x777b) == current_player) {
            if (uVar4 != 0xffff) {
              iVar8 = piVar6[iVar7];
              pbVar1 = (byte *)(iVar8 * 0x16 + -0x7772);
              *pbVar1 = *pbVar1 & 0xf0;
              pbVar1 = (byte *)(iVar8 * 0x16 + -0x7772);
              *pbVar1 = *pbVar1 | 1;
              iVar8 = piVar6[iVar7];
              pbVar1 = (byte *)(iVar8 * 0x16 + -0x7771);
              *pbVar1 = *pbVar1 & 1;
              pbVar1 = (byte *)(iVar8 * 0x16 + -0x7771);
              *pbVar1 = *pbVar1 | (char)uVar4 << 1;
            }
          }
          else {
            piVar6[iVar7] = -1;
          }
        }
      }
      local_1a = 2;
    }
  }
  return local_1a;
}


