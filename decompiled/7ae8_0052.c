
void __cdecl16far FUN_7ae8_0052(undefined2 param_1)

{
  int iVar1;
  int *piVar2;
  undefined2 in_DX;
  undefined2 *puVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined4 local_10;
  int local_c;
  int local_a;
  undefined4 local_8;
  int local_4;
  
  iVar5 = 8;
  local_4 = 8;
  piVar2 = (int *)FUN_7ae8_0198(param_1);
  local_8 = (int *)CONCAT22(in_DX,piVar2);
  FUN_7ae8_029c(1);
  for (iVar4 = 0; uVar6 = (undefined2)((ulong)local_8 >> 0x10), piVar2 = (int *)local_8,
      iVar4 < piVar2[4]; iVar4 = iVar4 + 1) {
    iVar1 = piVar2[1];
    puVar3 = (undefined2 *)(*local_8 + iVar4 * 0x2e);
    local_10 = (undefined2 *)CONCAT22(iVar1,puVar3);
    uVar6 = ((undefined2 *)&DAT_4125_1adc)[iVar4 * 6];
    puVar3[1] = ((undefined2 *)&DAT_4125_1ade)[iVar4 * 6];
    *local_10 = uVar6;
    uVar6 = ((undefined2 *)&DAT_4125_1ae0)[iVar4 * 6];
    puVar3[3] = ((undefined2 *)&DAT_4125_1ae2)[iVar4 * 6];
    puVar3[2] = uVar6;
    puVar3[4] = ((undefined2 *)&DAT_4125_1ae6)[iVar4 * 6];
    puVar3[5] = ((undefined2 *)&DAT_4125_1ae4)[iVar4 * 6];
    FUN_21e2_0ab8(*(undefined4 *)(puVar3 + 2),&local_a);
    local_a = (local_a + 7 >> 3) * 8;
    DAT_451b_0cf0 = local_c;
    puVar3[0x16] = local_c;
    puVar3[6] = local_a + local_4;
    puVar3[7] = iVar5;
    puVar3[8] = 0;
    puVar3[9] = local_a + local_4;
    puVar3[10] = local_c + 2;
    puVar3[0xb] = iVar5;
    puVar3[0xc] = local_c + 3;
    *(undefined4 *)(puVar3 + 0xd) = 0xffff;
    puVar3[0xf] = 1;
    *(undefined4 *)(puVar3 + 0x10) = 0;
    iVar5 = iVar5 + local_a + local_4 * 2;
  }
  DAT_451b_0cf2 = iVar5;
  FUN_2372_122f(piVar2,uVar6,0x3e,0x409d);
  return;
}


