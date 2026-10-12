
int __cdecl16far FUN_21e2_01f7(int param_1,undefined2 param_2,undefined2 param_3,int param_4)

{
  undefined2 *puVar1;
  undefined4 uVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 in_DX;
  undefined2 *puVar6;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  undefined2 uVar12;
  int local_18;
  undefined1 local_a [8];
  
  puVar1 = (undefined2 *)*(undefined4 *)((int)DAT_451b_30e9 + 0x61);
  iVar3 = FUN_21e2_0ae3(param_3);
  uVar9 = (undefined2)((ulong)puVar1 >> 0x10);
  puVar6 = (undefined2 *)puVar1;
  uVar10 = (undefined2)((ulong)DAT_3ef3_0000 >> 0x10);
  uVar2 = *(undefined4 *)((int)DAT_3ef3_0000 + 0x1c);
  uVar5 = *(undefined2 *)((int)uVar2 + iVar3 * 2);
  uVar2 = *(undefined4 *)((int)DAT_3ef3_0000 + 0x20);
  uVar10 = *(undefined2 *)((int)uVar2 + iVar3 * 2);
  if ((param_4 == 0) || (*(long *)(puVar6 + 2) == 0)) {
    iVar7 = param_1;
    uVar5 = FUN_216d_002e(local_a);
    FUN_24d0_009d(*puVar1,puVar6[1],uVar5,in_DX,iVar7,param_2);
  }
  else {
    uVar11 = (undefined2)((ulong)*(undefined4 *)(puVar6 + 2) >> 0x10);
    iVar7 = (int)*(undefined4 *)(puVar6 + 2);
    if ((*(int *)(iVar7 + 10) != 0 || *(int *)(iVar7 + 0xc) != 0) &&
       ((*(uint *)((int)*(undefined4 *)(puVar6 + 2) + 0x35) & 10) == 0)) {
      for (local_18 = 0; local_18 < (int)(uint)*(byte *)((int)*(undefined4 *)(puVar6 + 2) + 0x34);
          local_18 = local_18 + 1) {
        uVar11 = (undefined2)((ulong)*(undefined4 *)(puVar6 + 2) >> 0x10);
        iVar7 = (int)*(undefined4 *)(puVar6 + 2) + local_18 * 4;
        uVar11 = mem_lock(*(undefined2 *)(iVar7 + 10),*(undefined2 *)(iVar7 + 0xc));
        uVar12 = (undefined2)((ulong)*(undefined4 *)(puVar6 + 2) >> 0x10);
        iVar7 = (int)*(undefined4 *)(puVar6 + 2) + local_18 * 4;
        *(undefined2 *)(iVar7 + 0x1c) = in_DX;
        *(undefined2 *)(iVar7 + 0x1a) = uVar11;
      }
    }
    uVar11 = puVar6[3];
    uVar12 = puVar6[2];
    iVar7 = param_1;
    uVar4 = FUN_216d_002e(local_a);
    FUN_24d0_00f8(*puVar1,puVar6[1],uVar4,in_DX,iVar7,param_2,uVar12,uVar11,uVar5,uVar10);
    uVar5 = (undefined2)((ulong)*(undefined4 *)(puVar6 + 2) >> 0x10);
    iVar7 = (int)*(undefined4 *)(puVar6 + 2);
    if ((*(int *)(iVar7 + 10) != 0 || *(int *)(iVar7 + 0xc) != 0) &&
       ((*(uint *)((int)*(undefined4 *)(puVar6 + 2) + 0x35) & 10) == 0)) {
      for (iVar7 = 0; iVar7 < (int)(uint)*(byte *)((int)*(undefined4 *)(puVar6 + 2) + 0x34);
          iVar7 = iVar7 + 1) {
        uVar5 = (undefined2)((ulong)*(undefined4 *)(puVar6 + 2) >> 0x10);
        iVar8 = (int)*(undefined4 *)(puVar6 + 2) + iVar7 * 4;
        mem_unlock(*(undefined2 *)(iVar8 + 10),*(undefined2 *)(iVar8 + 0xc));
      }
    }
  }
  return param_1 + (uint)*(byte *)((int)*(undefined4 *)(puVar6 + 0xc) + iVar3);
}


