
void __cdecl16far FUN_1555_16f0(int param_1,undefined2 *param_2,int param_3)

{
  bool bVar1;
  undefined2 uVar2;
  char *pcVar3;
  undefined2 in_DX;
  int *piVar4;
  undefined2 *puVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  int local_8;
  char *local_6;
  int local_4;
  
  local_4 = -1;
  local_6 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  local_8 = -1;
  bVar1 = true;
  DAT_451b_0012._0_2_ = mem_lock();
  DAT_451b_0012._2_2_ = in_DX;
  if (param_3 == 0) {
    local_4 = 0;
  }
  else {
    iVar6 = 1;
    while ((iVar7 = local_4, iVar6 < 0x14 &&
           (piVar4 = (int *)((int)DAT_451b_0012 + iVar6 * 0xd6), iVar7 = iVar6, *piVar4 != 0))) {
      pcVar3 = (char *)map_distance(piVar4[5],piVar4[6],piVar4[3],piVar4[4]);
      if ((int)pcVar3 < (int)local_6) {
        local_8 = iVar6;
        local_6 = pcVar3;
      }
      iVar6 = iVar6 + 1;
    }
    local_4 = iVar7;
    if ((local_4 == -1) && (local_8 == -1)) {
      bVar1 = false;
    }
    if ((local_4 == -1) && ((int)local_6 < param_1)) {
      local_4 = local_8;
    }
    if (local_4 == -1) {
      bVar1 = false;
    }
  }
  if (bVar1) {
    local_4 = local_4 * 0xd6;
    iVar6 = 200;
    while( true ) {
      iVar7 = iVar6 + -1;
      puVar5 = (undefined2 *)param_2;
      uVar8 = (undefined2)((ulong)param_2 >> 0x10);
      if (iVar6 == 0) break;
      *(undefined1 *)((int)DAT_451b_0012 + local_4 + iVar7 + 0xe) =
           *(undefined1 *)((int)*(undefined4 *)(puVar5 + 6) + iVar7);
      iVar6 = iVar7;
    }
    *(undefined2 *)((int)DAT_451b_0012 + local_4) = 2;
    *(undefined2 *)((int)DAT_451b_0012 + local_4 + 2) = puVar5[5];
    uVar2 = DAT_451b_0012._2_2_;
    *(undefined2 *)((int)DAT_451b_0012 + local_4 + 4) = puVar5[4];
    *(undefined2 *)((int)DAT_451b_0012 + local_4 + 6) = *param_2;
    *(undefined2 *)((int)DAT_451b_0012 + local_4 + 8) = puVar5[1];
    *(undefined2 *)((int)DAT_451b_0012 + local_4 + 10) = puVar5[2];
    *(undefined2 *)((int)DAT_451b_0012 + local_4 + 0xc) = puVar5[3];
  }
  mem_unlock(DAT_451b_001e);
  return;
}


