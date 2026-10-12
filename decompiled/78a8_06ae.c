
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_78a8_06ae(int param_1,int param_2,int param_3,int param_4)

{
  undefined2 uVar1;
  undefined2 *puVar2;
  bool bVar3;
  int iVar4;
  int in_DX;
  int *piVar5;
  int iVar6;
  undefined2 uVar7;
  undefined4 local_16;
  undefined4 local_a;
  undefined4 local_6;
  
  if (param_1 == 0) {
    local_6 = 0x412547f8;
    local_a = DAT_451b_0894;
    in_DX = DAT_451b_08a7;
  }
  else if (param_1 == 1) {
    local_6 = 0x412547d7;
    local_a = _LAB_451b_0873;
    in_DX = DAT_451b_0886;
  }
  else if (param_1 == 2) {
    local_6 = 0x412547b6;
    local_a = _caseD_0_1;
    in_DX = _LAB_451b_0865;
  }
  bVar3 = false;
  iVar4 = 0;
  while( true ) {
    iVar6 = (int)local_6;
    uVar7 = (undefined2)((ulong)local_6 >> 0x10);
    if ((bVar3) || (in_DX <= iVar4)) break;
    if ((*(int *)((int)*(undefined4 *)(iVar6 + 0x19) + iVar4 * 6) == param_2) &&
       ((*(int *)((int)*(undefined4 *)(iVar6 + 0x19) + iVar4 * 6 + 2) == param_3 &&
        (*(int *)((int)*(undefined4 *)(iVar6 + 0x19) + iVar4 * 6 + 4) == param_4)))) {
      bVar3 = true;
    }
    iVar4 = iVar4 + 1;
  }
  if (bVar3) {
    iVar4 = iVar4 + -1;
  }
  else {
    iVar4 = in_DX + -1;
    puVar2 = (undefined2 *)*(undefined4 *)(iVar6 + 0x19);
    uVar1 = *(undefined2 *)(iVar6 + 0x1b);
    piVar5 = (int *)(*(int *)(iVar6 + 0x19) + iVar4 * 6);
    local_16 = (int *)CONCAT22(uVar1,piVar5);
    if (*local_16 != param_2) {
      *local_16 = param_2;
      FUN_78a8_0839(local_6,*puVar2,param_2);
    }
    uVar7 = (undefined2)((ulong)puVar2 >> 0x10);
    if (piVar5[1] != param_3) {
      piVar5[1] = param_3;
      FUN_78a8_0839(local_6,((undefined2 *)puVar2)[1],param_3);
    }
    if (piVar5[2] != param_4) {
      piVar5[2] = param_4;
      FUN_78a8_0839(local_6,((undefined2 *)puVar2)[2],param_4);
    }
  }
  DAT_3ef3_0000 = local_a;
  FUN_24b0_01f0((int)local_a + iVar4 * 0x2a,local_a._2_2_);
  return;
}


