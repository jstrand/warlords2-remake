
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_177b_01a1(int param_1,int param_2,int param_3,uint param_4)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int local_1e;
  int local_16;
  int local_14;
  uint local_e;
  
  bVar1 = false;
  if (_LAB_4125_1255_1 == 0) {
    iVar6 = (param_1 - DAT_3c04_0177) * 5 + 2;
    iVar7 = (param_2 - DAT_3c04_0175) * 0x28 + 0x1e;
    iVar2 = iVar6 * 8;
    iVar3 = FUN_177b_03f8();
    if (iVar3 != 0) {
      local_e = 1;
      iVar8 = ((param_3 % 4 << 6) >> 3) + (param_3 / 4) * 0x28 * DAT_451b_1763 * 2;
      iVar3 = iVar8;
      FUN_22bf_0abd();
      iVar4 = FUN_22bf_0b2a();
      if ((((local_14 <= iVar2 + _LAB_4125_1267_1) && (iVar2 - *(int *)(iVar4 + 4) <= local_14)) &&
          (local_16 <= iVar7 + _LAB_4125_1269_1)) && (iVar7 - *(int *)(iVar4 + 6) <= local_16)) {
        bVar1 = true;
      }
      if (bVar1) {
        FUN_22bf_07a5();
      }
      FUN_1e68_000a();
      if ((DAT_451b_1765 != 0) && ((DAT_451b_1790 & 10) == 0)) {
        for (iVar2 = 0; iVar2 < (int)(uint)DAT_451b_178f; iVar2 = iVar2 + 1) {
          uVar5 = mem_lock(((undefined4 *)&DAT_451b_1765)[iVar2]);
          *(int *)((undefined *)&DAT_451b_1777 + iVar2 * 4) = iVar3;
          ((undefined2 *)&DAT_451b_1775)[iVar2 * 2] = uVar5;
        }
      }
      if ((DAT_451b_16ef != 0) && ((DAT_451b_171a & 10) == 0)) {
        for (iVar2 = 0; iVar2 < (int)(uint)DAT_451b_1719; iVar2 = iVar2 + 1) {
          uVar5 = mem_lock(((undefined4 *)&DAT_451b_16ef)[iVar2]);
          *(int *)((undefined *)&DAT_451b_1701 + iVar2 * 4) = iVar3;
          ((undefined2 *)&DAT_451b_16ff)[iVar2 * 2] = uVar5;
        }
      }
      for (iVar2 = 0; iVar2 < (int)(uint)DAT_451b_178f; iVar2 = iVar2 + 1) {
        FUN_1e68_0018();
        local_1e = (int)*(undefined4 *)
                         ((undefined2 *)&DAT_451b_1775 + (uint)((local_e & param_4) == 0) * 2);
        FUN_1e68_0059((int)((ulong)*(undefined4 *)
                                    ((undefined2 *)&DAT_451b_1775 +
                                    (uint)((local_e & param_4) == 0) * 2) >> 0x10),local_1e + iVar8,
                      DAT_451b_1763,*(undefined2 *)((int)DAT_4125_34e7 + 8),iVar6,iVar7,0x280003,
                      (int)((ulong)_DAT_451b_16ff >> 0x10),local_1e + iVar8,DAT_451b_16ed);
        local_e = local_e << 1;
      }
      if ((DAT_451b_1765 != 0) && ((DAT_451b_1790 & 10) == 0)) {
        for (iVar2 = 0; iVar2 < (int)(uint)DAT_451b_178f; iVar2 = iVar2 + 1) {
          mem_unlock(((undefined4 *)&DAT_451b_1765)[iVar2]);
        }
      }
      if ((DAT_451b_16ef != 0) && ((DAT_451b_171a & 10) == 0)) {
        for (iVar2 = 0; iVar2 < (int)(uint)DAT_451b_1719; iVar2 = iVar2 + 1) {
          mem_unlock(((undefined4 *)&DAT_451b_16ef)[iVar2]);
        }
      }
      if (bVar1) {
        FUN_22bf_0757();
      }
    }
  }
  return;
}


