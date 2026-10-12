
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far path_prepare_grid(undefined4 param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  byte *pbVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  byte *pbVar9;
  int iVar10;
  int iVar11;
  undefined2 uVar12;
  undefined2 uVar13;
  undefined2 *local_16;
  int local_14;
  byte *local_12;
  undefined2 *local_10;
  int local_e;
  byte *local_c;
  undefined2 *local_a;
  int local_8;
  byte *local_6;
  
  bVar1 = true;
  DAT_451b_000e = 0;
  if (opt_hidden_map != 0) {
    iVar3 = is_computer_turn();
    if (iVar3 == 0) goto LAB_1555_08ed;
  }
  bVar1 = false;
LAB_1555_08ed:
  uVar12 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  uVar13 = (undefined2)((ulong)path_cost_grid >> 0x10);
  pbVar5 = (byte *)((int)path_cost_grid + *(int *)(iVar3 + 4) * 0x9c + *(int *)(iVar3 + 6));
  if (((*pbVar5 & 0x80) != 0) && ((*pbVar5 & 7) == 0)) {
    path_mark_cities(param_1,1);
  }
  if (*(int *)(iVar3 + 10) == 1) {
    DAT_451b_000e = 10;
    uVar8 = (uint)*(byte *)(*(int *)(iVar3 + 6) * 0xe0 + *(int *)(iVar3 + 4) * 2);
    if ((*(char *)(uVar8 + 0x710) == '\x03') || (*(char *)(uVar8 + 0x710) == '\x02')) {
      DAT_451b_000e = 0x14;
    }
    local_6 = (byte *)&DAT_451b_0550;
    local_8 = -0x7780;
    local_a = (undefined2 *)&DAT_451b_04e0;
    iVar6 = 0x70;
    do {
      local_6 = local_6 + -1;
      local_8 = local_8 + -0x138;
      local_a = local_a + -0x4e;
      iVar7 = iVar6 + -1;
      if (iVar6 == 0) {
        return;
      }
      iVar4 = 0x138;
      iVar2 = (int)path_cost_grid;
      iVar10 = 0x9c;
      pbVar5 = local_6;
LAB_1555_0a0a:
      iVar4 = iVar4 + -2;
      pbVar5 = pbVar5 + -0x70;
      iVar11 = iVar10 + -1;
      iVar6 = iVar7;
      if (iVar10 != 0) {
        *(undefined2 *)((int)_DAT_451b_001a + local_8 + iVar4) = 30000;
        iVar10 = iVar11;
        if ((!bVar1) || ((*pbVar5 & 0x20) != 0)) goto LAB_1555_09e9;
        if ((*(int *)(iVar3 + 4) == iVar7) && (*(int *)(iVar3 + 6) == iVar11)) goto LAB_1555_09e9;
        goto LAB_1555_09fc;
      }
    } while( true );
  }
  if (*(int *)(iVar3 + 10) == 0) {
    local_c = (byte *)&DAT_451b_0550;
    local_e = -0x7780;
    local_10 = (undefined2 *)&DAT_451b_04e0;
    iVar6 = 0x70;
    while( true ) {
      local_c = local_c + -1;
      local_e = local_e + -0x138;
      local_10 = local_10 + -0x4e;
      iVar7 = iVar6 + -1;
      if (iVar6 == 0) break;
      iVar4 = 0x138;
      iVar2 = (int)path_cost_grid;
      iVar10 = 0x9c;
      pbVar5 = local_c;
      while( true ) {
        iVar4 = iVar4 + -2;
        pbVar5 = pbVar5 + -0x70;
        iVar11 = iVar10 + -1;
        iVar6 = iVar7;
        if (iVar10 == 0) break;
        *(undefined2 *)((int)_DAT_451b_001a + local_e + iVar4) = 30000;
        iVar10 = iVar11;
        if (((((*(int *)(iVar3 + 4) != iVar7) && (*(int *)(iVar3 + 6) != iVar11)) && (bVar1)) &&
            ((*pbVar5 & 0x20) == 0)) || ((*(byte *)((int)local_10 + iVar11 + iVar2) & 8) == 0)) {
          *(undefined2 *)((int)_DAT_451b_001a + local_e + iVar4) = 0x7531;
        }
      }
    }
    return;
  }
  local_12 = (byte *)&DAT_451b_0550;
  local_14 = -0x7780;
  local_16 = (undefined2 *)&DAT_451b_04e0;
  iVar6 = 0x70;
  do {
    local_12 = local_12 + -1;
    local_14 = local_14 + -0x138;
    local_16 = local_16 + -0x4e;
    iVar7 = iVar6 + -1;
    if (iVar6 == 0) {
      return;
    }
    iVar4 = 0x138;
    iVar2 = (int)path_cost_grid;
    iVar10 = 0x9c;
    pbVar5 = local_12;
LAB_1555_0bb6:
    iVar4 = iVar4 + -2;
    pbVar5 = pbVar5 + -0x70;
    iVar11 = iVar10 + -1;
    iVar6 = iVar7;
    if (iVar10 != 0) {
      *(undefined2 *)((int)_DAT_451b_001a + local_14 + iVar4) = 30000;
      iVar10 = iVar11;
      if (((*(int *)(iVar3 + 4) == iVar7) || (*(int *)(iVar3 + 6) == iVar11)) ||
         ((!bVar1 || ((*pbVar5 & 0x20) != 0)))) goto LAB_1555_0b8d;
      goto LAB_1555_0ba8;
    }
  } while( true );
LAB_1555_09e9:
  if ((*(byte *)((int)local_a + iVar11 + iVar2) & 7) == 0) {
LAB_1555_09fc:
    *(undefined2 *)((int)_DAT_451b_001a + local_8 + iVar4) = 0x7531;
  }
  goto LAB_1555_0a0a;
LAB_1555_0b8d:
  pbVar9 = (byte *)((int)local_16 + iVar11 + iVar2);
  if (((*pbVar9 & 0x80) != 0) && ((*pbVar9 & 7) == 0)) {
LAB_1555_0ba8:
    *(undefined2 *)((int)_DAT_451b_001a + local_14 + iVar4) = 0x7531;
  }
  goto LAB_1555_0bb6;
}


