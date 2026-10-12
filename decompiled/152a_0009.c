
void __cdecl16far
FUN_152a_0009(undefined4 param_1,int param_2,int param_3,undefined4 param_4,int param_5,int param_6)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  undefined4 local_a;
  
  for (iVar4 = 0; uVar8 = (undefined2)((ulong)param_1 >> 0x10), iVar3 = (int)param_1,
      iVar4 < (int)(uint)*(byte *)(iVar3 + 0x34); iVar4 = iVar4 + 1) {
    FUN_1e68_0018(iVar4);
    if (((*(uint *)(iVar3 + 0x35) & 1) == 0) || (param_3 < *(int *)(iVar3 + 6))) {
      iVar1 = iVar3 + iVar4 * 4;
      uVar6 = *(undefined2 *)(iVar1 + 0x1c);
      iVar1 = *(int *)(iVar1 + 0x1a) + param_3 * *(int *)(iVar3 + 8) * 2 +
              (param_2 >> (DAT_451b_2a3e & 0x1f));
    }
    else {
      uVar5 = (undefined2)((ulong)*(undefined4 *)(iVar3 + 0x37) >> 0x10);
      iVar1 = (int)*(undefined4 *)(iVar3 + 0x37) + iVar4 * 4;
      uVar6 = *(undefined2 *)(iVar1 + 0x1c);
      iVar1 = *(int *)(iVar1 + 0x1a) +
              (param_3 - *(int *)(iVar3 + 6)) * *(int *)((int)*(undefined4 *)(iVar3 + 0x37) + 8) * 2
              + (param_2 >> (DAT_451b_2a3e & 0x1f));
    }
    local_a = CONCAT22(uVar6,iVar1);
    uVar6 = (undefined2)((ulong)param_4 >> 0x10);
    iVar1 = (int)param_4;
    if (((*(uint *)(iVar1 + 0x35) & 1) == 0) || (param_6 < *(int *)(iVar1 + 6))) {
      iVar2 = iVar1 + iVar4 * 4;
      uVar5 = *(undefined2 *)(iVar2 + 0x1c);
      iVar2 = *(int *)(iVar2 + 0x1a) + param_6 * *(int *)(iVar1 + 8) * 2 +
              (param_5 >> (DAT_451b_2a3e & 0x1f));
    }
    else {
      uVar7 = (undefined2)((ulong)*(undefined4 *)(iVar1 + 0x37) >> 0x10);
      iVar2 = (int)*(undefined4 *)(iVar1 + 0x37) + iVar4 * 4;
      uVar5 = *(undefined2 *)(iVar2 + 0x1c);
      iVar2 = *(int *)(iVar2 + 0x1a) +
              (param_6 - *(int *)(iVar1 + 6)) * *(int *)((int)*(undefined4 *)(iVar1 + 0x37) + 8) * 2
              + (param_5 >> (DAT_451b_2a3e & 0x1f));
    }
    (*DAT_451b_0000)(0x1e68,local_a,iVar2,uVar5,0x28,5,*(int *)(iVar3 + 8) * 2 + -5,
                     *(int *)(iVar1 + 8) * 2 + -5);
  }
  return;
}


