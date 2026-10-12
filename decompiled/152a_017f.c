
void __cdecl16far
FUN_152a_017f(undefined4 param_1,int param_2,int param_3,undefined4 param_4,int param_5,int param_6,
             undefined4 param_7,int param_8,int param_9)

{
  int iVar1;
  int iVar2;
  int iVar3;
  byte bVar4;
  byte bVar5;
  byte bVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  undefined2 uVar11;
  undefined2 uVar12;
  undefined2 uVar13;
  
  uVar11 = (undefined2)((ulong)param_1 >> 0x10);
  iVar7 = (int)param_1;
  iVar1 = *(int *)(iVar7 + 8);
  bVar4 = DAT_451b_2a3e & 0x1f;
  uVar12 = (undefined2)((ulong)param_4 >> 0x10);
  iVar8 = (int)param_4;
  iVar2 = *(int *)(iVar8 + 8);
  bVar5 = DAT_451b_2a3e & 0x1f;
  uVar13 = (undefined2)((ulong)param_7 >> 0x10);
  iVar9 = (int)param_7;
  iVar3 = *(int *)(iVar9 + 8);
  bVar6 = DAT_451b_2a3e & 0x1f;
  for (iVar10 = 0; iVar10 < (int)(uint)*(byte *)(iVar7 + 0x34); iVar10 = iVar10 + 1) {
    FUN_1e68_0018(iVar10);
    FUN_1e68_00f3((int)((ulong)*(undefined4 *)(iVar7 + iVar10 * 4 + 0x1a) >> 0x10),
                  param_3 * iVar1 * 2 + (param_2 >> bVar4),*(undefined2 *)(iVar7 + 8),
                  (int)((ulong)*(undefined4 *)(iVar8 + iVar10 * 4 + 0x1a) >> 0x10),
                  param_6 * iVar2 * 2 + (param_5 >> bVar5),*(undefined2 *)(iVar8 + 8),
                  0x30 >> (DAT_451b_2a40 & 0x1f),0x28,
                  (int)((ulong)*(undefined4 *)(iVar9 + 0x1a) >> 0x10),
                  param_9 * iVar3 * 2 + (param_8 >> bVar6),*(undefined2 *)(iVar9 + 8));
  }
  return;
}


