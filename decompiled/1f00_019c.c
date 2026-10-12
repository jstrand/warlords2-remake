
uint __cdecl16far FUN_1f00_019c(undefined4 param_1,int param_2,int param_3)

{
  bool bVar1;
  byte bVar2;
  byte *pbVar3;
  undefined2 in_DX;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  undefined4 local_c;
  uint local_6;
  
  bVar2 = 0;
  local_6 = 0;
  uVar7 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  uVar8 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
  if ((*(uint *)(iVar4 + 4) < *(uint *)((int)DAT_4125_34e7 + 4)) ||
     (*(uint *)(iVar4 + 6) < *(uint *)((int)DAT_4125_34e7 + 6))) {
    bVar1 = false;
  }
  else {
    bVar1 = true;
  }
  if (((!bVar1) && (*(long *)(iVar4 + 10) != 0)) && ((*(uint *)(iVar4 + 0x35) & 10) == 0)) {
    for (iVar6 = 0; iVar6 < (int)(uint)*(byte *)(iVar4 + 0x34); iVar6 = iVar6 + 1) {
      iVar5 = iVar4 + iVar6 * 4;
      uVar8 = mem_lock(*(undefined2 *)(iVar5 + 10),*(undefined2 *)(iVar5 + 0xc));
      iVar5 = iVar4 + iVar6 * 4;
      *(undefined2 *)(iVar5 + 0x1c) = in_DX;
      *(undefined2 *)(iVar5 + 0x1a) = uVar8;
    }
  }
  for (iVar6 = 0; iVar6 < (int)(uint)*(byte *)(iVar4 + 0x34); iVar6 = iVar6 + 1) {
    if (((bVar1) &&
        (iVar5 = iVar4 + iVar6 * 4, *(int *)(iVar5 + 10) != 0 || *(int *)(iVar5 + 0xc) != 0)) &&
       ((*(uint *)(iVar4 + 0x35) & 10) == 0)) {
      iVar5 = iVar4 + iVar6 * 4;
      uVar8 = mem_lock(*(undefined2 *)(iVar5 + 10),*(undefined2 *)(iVar5 + 0xc));
      iVar5 = iVar4 + iVar6 * 4;
      *(undefined2 *)(iVar5 + 0x1c) = in_DX;
      *(undefined2 *)(iVar5 + 0x1a) = uVar8;
    }
    FUN_1e68_0018(iVar6);
    if (((*(uint *)(iVar4 + 0x35) & 1) == 0) || (param_3 < *(int *)(iVar4 + 6))) {
      iVar5 = iVar4 + iVar6 * 4;
      in_DX = *(undefined2 *)(iVar5 + 0x1c);
      pbVar3 = (byte *)(*(int *)(iVar5 + 0x1a) + param_3 * *(int *)(iVar4 + 8) * 2 +
                       (param_2 >> (DAT_451b_2a3e & 0x1f)));
    }
    else {
      uVar8 = (undefined2)((ulong)*(undefined4 *)(iVar4 + 0x37) >> 0x10);
      iVar5 = (int)*(undefined4 *)(iVar4 + 0x37) + iVar6 * 4;
      in_DX = *(undefined2 *)(iVar5 + 0x1c);
      pbVar3 = (byte *)(*(int *)(iVar5 + 0x1a) +
                        (param_3 - *(int *)(iVar4 + 6)) *
                        *(int *)((int)*(undefined4 *)(iVar4 + 0x37) + 8) * 2 +
                       (param_2 >> (DAT_451b_2a3e & 0x1f)));
    }
    local_c = (byte *)CONCAT22(in_DX,pbVar3);
    local_6 = local_6 | (uint)((*local_c & '\x01' << (7 - ((byte)param_2 & 7) & 0x1f)) != 0) <<
                        (bVar2 & 0x1f);
    bVar2 = bVar2 + 1;
    iVar5 = iVar4 + iVar6 * 4;
    if ((*(int *)(iVar5 + 10) != 0 || *(int *)(iVar5 + 0xc) != 0) &&
       ((*(uint *)(iVar4 + 0x35) & 10) == 0)) {
      iVar5 = iVar4 + iVar6 * 4;
      mem_unlock(*(undefined2 *)(iVar5 + 10),*(undefined2 *)(iVar5 + 0xc));
    }
  }
  return local_6;
}


