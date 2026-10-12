
int __cdecl16far
FUN_21e2_0553(int param_1,undefined2 param_2,undefined2 param_3,int param_4,undefined4 param_5)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 *puVar3;
  undefined4 uVar4;
  int iVar5;
  undefined2 in_DX;
  undefined2 *puVar6;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  undefined2 uVar12;
  undefined2 uVar13;
  undefined2 uVar14;
  int local_10;
  
  uVar14 = 0x21e2;
  puVar3 = (undefined2 *)*(undefined4 *)((int)DAT_451b_30e9 + 0x61);
  iVar5 = FUN_21e2_0ae3(param_3);
  uVar9 = (undefined2)((ulong)puVar3 >> 0x10);
  puVar6 = (undefined2 *)puVar3;
  uVar13 = *(undefined2 *)((int)*(undefined4 *)(puVar6 + 0xe) + iVar5 * 2);
  uVar1 = *(undefined2 *)((int)*(undefined4 *)(puVar6 + 0x10) + iVar5 * 2);
  uVar10 = (undefined2)((ulong)DAT_3ef3_0000 >> 0x10);
  uVar4 = *(undefined4 *)((int)DAT_3ef3_0000 + 0x1c);
  uVar2 = *(undefined2 *)((int)uVar4 + iVar5 * 2);
  uVar4 = *(undefined4 *)((int)DAT_3ef3_0000 + 0x20);
  uVar10 = *(undefined2 *)((int)uVar4 + iVar5 * 2);
  if ((param_4 == 0) || (*(long *)(puVar6 + 2) == 0)) {
    (*DAT_451b_2a74)(0x21e2,*puVar3,puVar6[1],uVar13,uVar1,
                     *(undefined1 *)((int)*(undefined4 *)(puVar6 + 10) + iVar5),puVar6[0x12],
                     (int)param_5,param_5._2_2_,param_1,param_2);
  }
  else {
    uVar11 = (undefined2)((ulong)*(undefined4 *)(puVar6 + 2) >> 0x10);
    iVar7 = (int)*(undefined4 *)(puVar6 + 2);
    if ((*(int *)(iVar7 + 10) != 0 || *(int *)(iVar7 + 0xc) != 0) &&
       ((*(uint *)((int)*(undefined4 *)(puVar6 + 2) + 0x35) & 10) == 0)) {
      for (local_10 = 0; local_10 < (int)(uint)*(byte *)((int)*(undefined4 *)(puVar6 + 2) + 0x34);
          local_10 = local_10 + 1) {
        uVar11 = (undefined2)((ulong)*(undefined4 *)(puVar6 + 2) >> 0x10);
        iVar7 = (int)*(undefined4 *)(puVar6 + 2) + local_10 * 4;
        uVar14 = 0x1d28;
        uVar11 = mem_lock(*(undefined2 *)(iVar7 + 10),*(undefined2 *)(iVar7 + 0xc));
        uVar12 = (undefined2)((ulong)*(undefined4 *)(puVar6 + 2) >> 0x10);
        iVar7 = (int)*(undefined4 *)(puVar6 + 2) + local_10 * 4;
        *(undefined2 *)(iVar7 + 0x1c) = in_DX;
        *(undefined2 *)(iVar7 + 0x1a) = uVar11;
      }
    }
    if ((*(long *)((int)param_5 + 10) != 0) && ((*(uint *)((int)param_5 + 0x35) & 10) == 0)) {
      for (local_10 = 0; local_10 < (int)(uint)*(byte *)((int)param_5 + 0x34);
          local_10 = local_10 + 1) {
        iVar7 = (int)param_5 + local_10 * 4;
        uVar14 = 0x1d28;
        uVar11 = mem_lock(*(undefined2 *)(iVar7 + 10),*(undefined2 *)(iVar7 + 0xc));
        iVar7 = (int)param_5 + local_10 * 4;
        *(undefined2 *)(iVar7 + 0x1c) = in_DX;
        *(undefined2 *)(iVar7 + 0x1a) = uVar11;
      }
    }
    (*DAT_451b_2a68)(uVar14,*puVar3,puVar6[1],uVar13,uVar1,
                     *(undefined1 *)((int)*(undefined4 *)(puVar6 + 10) + iVar5),puVar6[0x12],
                     puVar6[2],puVar6[3],uVar2,uVar10,(int)param_5,param_5._2_2_,param_1,param_2);
    if ((*(long *)((int)param_5 + 10) != 0) && ((*(uint *)((int)param_5 + 0x35) & 10) == 0)) {
      for (iVar7 = 0; iVar7 < (int)(uint)*(byte *)((int)param_5 + 0x34); iVar7 = iVar7 + 1) {
        iVar8 = (int)param_5 + iVar7 * 4;
        mem_unlock(*(undefined2 *)(iVar8 + 10),*(undefined2 *)(iVar8 + 0xc));
      }
    }
    uVar13 = (undefined2)((ulong)*(undefined4 *)(puVar6 + 2) >> 0x10);
    iVar7 = (int)*(undefined4 *)(puVar6 + 2);
    if ((*(int *)(iVar7 + 10) != 0 || *(int *)(iVar7 + 0xc) != 0) &&
       ((*(uint *)((int)*(undefined4 *)(puVar6 + 2) + 0x35) & 10) == 0)) {
      for (iVar7 = 0; iVar7 < (int)(uint)*(byte *)((int)*(undefined4 *)(puVar6 + 2) + 0x34);
          iVar7 = iVar7 + 1) {
        uVar13 = (undefined2)((ulong)*(undefined4 *)(puVar6 + 2) >> 0x10);
        iVar8 = (int)*(undefined4 *)(puVar6 + 2) + iVar7 * 4;
        mem_unlock(*(undefined2 *)(iVar8 + 10),*(undefined2 *)(iVar8 + 0xc));
      }
    }
  }
  return param_1 + (uint)*(byte *)((int)*(undefined4 *)(puVar6 + 0xc) + iVar5);
}


