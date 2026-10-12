
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl16far path_step_costs(undefined2 *param_1,int param_2,undefined1 *param_3)

{
  int iVar1;
  int in_DX;
  undefined2 *puVar2;
  undefined1 *puVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 local_e;
  uint local_c;
  int local_a;
  int local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  puVar2 = (undefined2 *)param_1;
  local_4 = *param_1;
  local_6 = puVar2[1];
  local_8 = 0;
  local_a = 0;
  local_c = puVar2[4] & 8;
  local_e = 0;
  if (param_3 != (undefined1 *)0x0) {
    puVar3 = (undefined1 *)param_3 + 100;
    iVar4 = 100;
    while( true ) {
      puVar3 = puVar3 + -1;
      in_DX = iVar4 + -1;
      if (iVar4 == 0) break;
      *puVar3 = 0xff;
      iVar4 = in_DX;
    }
  }
  DAT_451b_001a = mem_lock(DAT_451b_0026);
  _DAT_451b_001c = in_DX;
  path_cost_grid._0_2_ = mem_lock(DAT_451b_0022);
  iVar4 = 0;
  puVar3 = (undefined1 *)param_3;
  path_cost_grid._2_2_ = in_DX;
  do {
    if ((*(char *)((int)*(undefined4 *)(puVar2 + 6) + iVar4) == -1) ||
       (iVar1 = FUN_1a8b_0b4d(&local_4), iVar1 == 0)) break;
    iVar1 = path_step_cost(param_1,local_4,local_6,local_c,&local_e);
    iVar1 = local_a + iVar1;
    if ((param_3 != (undefined1 *)0x0) && (iVar4 < 100)) {
      if (iVar1 < 0xff) {
        local_a._0_1_ = (undefined1)iVar1;
      }
      else {
        local_a._0_1_ = 0xff;
      }
      *puVar3 = (undefined1)local_a;
    }
    if (iVar1 <= param_2) {
      local_8 = local_8 + 1;
    }
    puVar3 = puVar3 + 1;
    iVar4 = iVar4 + 1;
    local_a = iVar1;
  } while (iVar4 < 100);
  mem_unlock(DAT_451b_0026);
  mem_unlock(DAT_451b_0022);
  return local_8;
}


