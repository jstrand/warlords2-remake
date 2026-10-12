
void __cdecl16far path_mark_cities(undefined4 param_1,int param_2)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar8;
  undefined2 local_6;
  undefined2 local_4;
  int iVar7;
  
  if ((DAT_4125_01de != 0) || (param_2 != 0)) {
    iVar2 = DAT_451b_000a;
    if (param_2 != 0) {
      DAT_451b_000a = -1;
      uVar8 = (undefined2)((ulong)param_1 >> 0x10);
      iVar3 = (int)param_1;
      iVar2 = DAT_451b_000a;
      if (*(char *)(*(byte *)(*(int *)(iVar3 + 6) * 0xe0 + *(int *)(iVar3 + 4) * 2) + 0x710) == '\n'
         ) {
        iVar4 = city_count;
        iVar6 = city_count * 0x41;
        do {
          iVar7 = iVar6;
          iVar6 = iVar7 + -0x41;
          iVar5 = iVar4 + -1;
          iVar2 = DAT_451b_000a;
          if (iVar4 == 0) break;
          iVar4 = iVar5;
        } while ((((*(int *)(iVar3 + 4) < *(int *)(iVar7 + 0x153c)) ||
                  (*(int *)(iVar7 + 0x153c) + 1 < *(int *)(iVar3 + 4))) ||
                 (*(int *)(iVar3 + 6) < *(int *)(iVar7 + 0x153e))) ||
                (iVar2 = iVar5, *(int *)(iVar7 + 0x153e) + 1 < *(int *)(iVar3 + 6)));
      }
    }
    DAT_451b_000a = iVar2;
    if (DAT_451b_000a != -1) {
      DAT_4125_01de = param_2;
      local_4 = 4;
      local_6 = 8;
      while( true ) {
        if (local_4 == 0) break;
        iVar2 = *(int *)(DAT_451b_000a * 0x41 + 0x157d) + *(int *)(local_6 + 0x1cc);
        iVar3 = *(int *)(DAT_451b_000a * 0x41 + 0x157f) + *(int *)(local_6 + 0x1d4);
        *(undefined1 *)((int)path_cost_grid + iVar2 * 0x9c + iVar3) = LAB_4125_1287_1;
        if (param_2 == 0) {
          if ((*(byte *)(iVar3 * 0xe0 + iVar2 * 2 + 1) & 0xf) == current_player) {
            iVar4 = FUN_1555_109d(iVar2,iVar3);
            if (iVar4 != 0) {
              pbVar1 = (byte *)((int)path_cost_grid + iVar2 * 0x9c + iVar3);
              *pbVar1 = *pbVar1 | 0x18;
            }
          }
          else {
            *(undefined1 *)((int)path_cost_grid + iVar2 * 0x9c + iVar3) = 0;
          }
        }
        else {
          iVar4 = FUN_1555_109d(iVar2,iVar3);
          if (iVar4 != 0) {
            pbVar1 = (byte *)((int)path_cost_grid + iVar2 * 0x9c + iVar3);
            *pbVar1 = *pbVar1 | 0x18;
          }
        }
        pbVar1 = (byte *)((int)path_cost_grid + iVar2 * 0x9c + iVar3);
        *pbVar1 = *pbVar1 | 0x80;
        local_6 = local_6 + -2;
        local_4 = local_4 + -1;
      }
    }
  }
  return;
}


