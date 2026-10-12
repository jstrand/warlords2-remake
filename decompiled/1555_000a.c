
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl16far pathfind(int *param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  undefined2 uVar7;
  int local_4;
  
  iVar6 = 0;
  uVar7 = (undefined2)((ulong)param_1 >> 0x10);
  piVar4 = (int *)param_1;
  iVar1 = map_distance(piVar4[2],piVar4[3],*param_1,piVar4[1]);
  if ((((piVar4[2] < 0) || (piVar4[3] < 0)) || (0x6f < piVar4[2])) || (0x9b < piVar4[3])) {
    return 0;
  }
  if (DAT_4125_00de == 0) {
    path_init();
  }
  iVar2 = 200;
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    *(undefined1 *)((int)*(undefined4 *)(piVar4 + 6) + iVar3) = 0xff;
    iVar2 = iVar3;
  }
  if ((piVar4[4] & 0x10U) == 0) {
    iVar2 = FUN_1555_1508(piVar4,uVar7,0);
    if (iVar2 != 0) {
      return 1;
    }
    if ((0xe < iVar1) && (iVar2 = FUN_1555_1508(piVar4,uVar7,1), iVar2 != 0)) {
      return 1;
    }
    if ((iVar1 == 1) && (iVar2 = path_single_step(piVar4,uVar7), iVar2 != 0)) {
      return 1;
    }
  }
  DAT_451b_001a = mem_lock((int)DAT_451b_0026,(int)((ulong)DAT_451b_0026 >> 0x10));
  _DAT_451b_001c = iVar3;
  path_cost_grid._0_2_ = mem_lock((int)DAT_451b_0022,(int)((ulong)DAT_451b_0022 >> 0x10));
  path_cost_grid._2_2_ = iVar3;
  path_prepare_grid(piVar4,uVar7);
  piVar5 = (int *)(DAT_451b_001a + piVar4[2] * 0x138 + piVar4[3] * 2);
  if ((*piVar5 != 0x7531) ||
     ((*(byte *)((int)path_cost_grid + piVar4[2] * 0x9c + piVar4[3]) & 0x80) != 0)) {
    *piVar5 = -1;
    for (iVar2 = 0; (iVar6 == 0 && (iVar2 < 2)); iVar2 = iVar2 + 1) {
      iVar6 = path_wavefront(piVar4,uVar7,iVar2);
    }
    if ((iVar6 == 1) && ((piVar4[4] & 0x10U) == 0)) {
      path_trace(piVar4,uVar7);
      if (local_4 == 0xd) {
        FUN_1555_13e4(piVar4,uVar7);
      }
      if (iVar1 < 0xf) {
        FUN_1555_16f0(iVar1,piVar4,uVar7,0);
      }
      else {
        FUN_1555_16f0(iVar1,piVar4,uVar7,1);
      }
    }
  }
  path_mark_cities(piVar4,uVar7,0);
  mem_unlock((int)DAT_451b_0026,(int)((ulong)DAT_451b_0026 >> 0x10));
  mem_unlock((int)DAT_451b_0022,(int)((ulong)DAT_451b_0022 >> 0x10));
  return iVar6;
}


