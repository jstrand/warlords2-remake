
void __cdecl16far FUN_2012_07cc(undefined4 param_1,uint param_2)

{
  long lVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 in_DX;
  int iVar4;
  int iVar5;
  uint uVar6;
  undefined2 uVar7;
  undefined4 local_e;
  uint local_6;
  int local_4;
  
  uVar2 = FUN_2012_02d8();
  iVar4 = (int)param_1;
  uVar7 = (undefined2)((ulong)param_1 >> 0x10);
  if ((((*DAT_4125_34e7 == 4) || (*DAT_4125_34e7 == 0)) || (*DAT_4125_34e7 == 1)) ||
     (*DAT_4125_34e7 == 2)) {
    local_6 = 1;
    for (uVar6 = 0; uVar6 < *(byte *)(iVar4 + 0x34); uVar6 = uVar6 + 1) {
      iVar5 = iVar4 + uVar6 * 4;
      if ((*(int *)(iVar5 + 10) != 0 || *(int *)(iVar5 + 0xc) != 0) &&
         ((*(uint *)(iVar4 + 0x35) & 10) == 0)) {
        uVar2 = mem_lock(*(undefined4 *)(iVar4 + uVar6 * 4 + 10));
        iVar5 = iVar4 + uVar6 * 4;
        *(undefined2 *)(iVar5 + 0x1c) = in_DX;
        *(undefined2 *)(iVar5 + 0x1a) = uVar2;
      }
      iVar5 = iVar4 + uVar6 * 4;
      local_e = (undefined2 *)
                CONCAT22(*(undefined2 *)(iVar5 + 0x1c),(undefined2 *)*(undefined2 *)(iVar5 + 0x1a));
      FUN_1e68_0018();
      if ((param_2 & local_6) == 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = 0xffff;
      }
      lVar1 = (long)*(int *)(iVar4 + 8) * (long)*(int *)(iVar4 + 6);
      in_DX = (undefined2)((ulong)lVar1 >> 0x10);
      local_4 = (int)lVar1;
      while (local_4 != 0) {
        *local_e = uVar2;
        local_e = (undefined2 *)CONCAT22(local_e._2_2_,(undefined2 *)local_e + 1);
        local_4 = local_4 + -1;
      }
      local_6 = local_6 << 1;
      iVar5 = iVar4 + uVar6 * 4;
      if ((*(int *)(iVar5 + 10) != 0 || *(int *)(iVar5 + 0xc) != 0) &&
         ((*(uint *)(iVar4 + 0x35) & 10) == 0)) {
        mem_unlock(*(undefined4 *)(iVar4 + uVar6 * 4 + 10));
      }
    }
  }
  else {
    if ((*(long *)(iVar4 + 10) != 0) && ((*(uint *)(iVar4 + 0x35) & 10) == 0)) {
      uVar3 = mem_lock(*(undefined4 *)(iVar4 + 10));
      *(undefined2 *)(iVar4 + 0x1c) = in_DX;
      *(undefined2 *)(iVar4 + 0x1a) = uVar3;
    }
    local_e = (undefined2 *)*(undefined4 *)(iVar4 + 0x1a);
    local_4 = *(int *)(iVar4 + 8) * *(int *)(iVar4 + 6);
    while (local_4 != 0) {
      *local_e = uVar2;
      local_e = (undefined2 *)CONCAT22(local_e._2_2_,(undefined2 *)local_e + 1);
      local_4 = local_4 + -1;
    }
    if ((*(long *)(iVar4 + 10) != 0) && ((*(uint *)(iVar4 + 0x35) & 10) == 0)) {
      mem_unlock(*(undefined4 *)(iVar4 + 10));
    }
  }
  return;
}


