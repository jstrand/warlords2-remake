
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far path_build_cost_grid(int player,int force)

{
  byte *pbVar1;
  byte *pbVar2;
  byte *pbVar3;
  undefined1 uVar4;
  undefined2 uVar5;
  int iVar6;
  int *piVar7;
  int iVar8;
  undefined2 in_DX;
  int iVar9;
  int iVar10;
  undefined2 *puVar11;
  int *local_10;
  byte *local_e;
  byte *local_c;
  byte *local_a;
  byte *local_8;
  int local_6;
  int local_4;
  
  if (DAT_4125_00de == 0) {
    path_init();
  }
  if ((force == 0) && (DAT_451b_000c == player)) {
    return;
  }
  DAT_451b_000c = player;
  DAT_4125_00e0 = (uint)(player == 0xe);
  DAT_451b_001a = mem_lock(DAT_451b_0026);
  _DAT_451b_001c = in_DX;
  uVar5 = mem_lock(DAT_451b_0022);
  path_cost_grid = CONCAT22(in_DX,uVar5);
  local_4 = 0x70;
  local_c = (byte *)0x8960;
  local_e = (byte *)&DAT_451b_0550;
  puVar11 = (undefined2 *)&DAT_451b_04e0;
  do {
    local_c = local_c + -2;
    local_e = local_e + -1;
    puVar11 = puVar11 + -0x4e;
    iVar8 = local_4 + -1;
    if (local_4 == 0) {
      local_6 = city_count;
      piVar7 = (int *)(city_count * 0x41 + 0x157f);
      local_10 = (int *)(city_count * 0x41 + 0x157d);
      while( true ) {
        piVar7 = (int *)((int)piVar7 + -0x41);
        local_10 = (int *)((int)local_10 + -0x41);
        iVar8 = local_6 + -1;
        if (local_6 == 0) break;
        local_6 = iVar8;
        if ((*(char *)(*(byte *)(*piVar7 * 0xe0 + *local_10 * 2) + 0x710) != '\n') &&
           (iVar8 = FUN_1555_109d(*local_10,*piVar7), iVar8 != 0)) {
          iVar8 = *piVar7;
          iVar6 = *local_10 * 0x9c;
          uVar5 = (undefined2)((ulong)path_cost_grid >> 0x10);
          pbVar2 = (byte *)((int)path_cost_grid + iVar6 + iVar8);
          *pbVar2 = *pbVar2 | 0x18;
          pbVar2 = (byte *)(iVar6 + (int)path_cost_grid + iVar8 + 0x9c);
          *pbVar2 = *pbVar2 | 0x18;
          pbVar2 = (byte *)((int)path_cost_grid + iVar6 + iVar8 + 1);
          *pbVar2 = *pbVar2 | 0x18;
          pbVar2 = (byte *)(iVar6 + (int)path_cost_grid + iVar8 + 0x9d);
          *pbVar2 = *pbVar2 | 0x18;
        }
      }
      mem_unlock(DAT_451b_0026);
      mem_unlock(DAT_451b_0022);
      return;
    }
    iVar6 = 0x9c;
    local_a = local_e;
    local_8 = local_c;
    while( true ) {
      pbVar3 = local_8 + -0xe0;
      local_a = local_a + -0x70;
      iVar10 = iVar6 + -1;
      local_4 = iVar8;
      if (iVar6 == 0) break;
      iVar6 = (int)*(char *)(*pbVar3 + 0x710);
      uVar5 = (undefined2)((ulong)path_cost_grid >> 0x10);
      if (player == 0xe) {
        if ((*local_a & 0x1f) == 0) {
          uVar4 = *(undefined1 *)(iVar6 * 2 + 0x1e0);
        }
        else {
          uVar4 = 1;
        }
        *(undefined1 *)((int)puVar11 + iVar10 + (int)path_cost_grid) = uVar4;
      }
      else {
        if ((*local_a & 0x1f) == 0) {
          uVar4 = ((undefined1 *)&terrain_move_cost)[iVar6 * 2];
        }
        else {
          uVar4 = 1;
        }
        *(undefined1 *)((int)puVar11 + iVar10 + (int)path_cost_grid) = uVar4;
      }
      if (iVar6 - 1U < 10) {
                    /* WARNING: Switch is manually overridden */
        iVar9 = (int)path_cost_grid;
        uVar5 = (undefined2)((ulong)path_cost_grid >> 0x10);
        switch((undefined4)*(undefined2 *)((iVar6 - 1U) * 2 + 0x1089)) {
        case 0x16402:
          if ((local_8[-0xdf] & 0xf) == player) {
            iVar6 = FUN_1555_109d(iVar8,iVar10);
            if (iVar6 != 0) {
              pbVar2 = (byte *)((int)puVar11 + iVar10 + (int)path_cost_grid);
              *pbVar2 = *pbVar2 | 0x18;
            }
          }
          else {
            *(undefined1 *)((int)puVar11 + iVar10 + iVar9) = 0;
          }
          pbVar2 = (byte *)((int)puVar11 + iVar10 + (int)path_cost_grid);
          *pbVar2 = *pbVar2 | 0x80;
          break;
        case 0x1644f:
          pbVar2 = (byte *)((int)puVar11 + iVar10 + iVar9);
          *pbVar2 = *pbVar2 | 0x10;
        case 0x1645b:
          pbVar2 = (byte *)((int)puVar11 + iVar10 + (int)path_cost_grid);
          *pbVar2 = *pbVar2 | 8;
          break;
        case 0x16469:
          pbVar2 = (byte *)((int)puVar11 + iVar10 + iVar9);
          *pbVar2 = *pbVar2 | 0x40;
          break;
        case 0x16477:
          pbVar2 = (byte *)((int)puVar11 + iVar10 + iVar9);
          *pbVar2 = *pbVar2 | 0x20;
        }
      }
      pbVar1 = local_8 + -0xdf;
      iVar6 = iVar10;
      local_8 = pbVar3;
      if ((char)*pbVar1 < '\0') {
        pbVar2 = (byte *)((int)puVar11 + iVar10 + (int)path_cost_grid);
        *pbVar2 = *pbVar2 | 0x10;
      }
    }
  } while( true );
}


