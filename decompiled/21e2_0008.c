
undefined4 * __cdecl16far
auto_str_font_can_t_create
          (undefined4 *param_1,undefined4 param_2,undefined4 param_3,int *param_4,char param_5,
          char param_6,undefined4 param_7,undefined4 param_8,byte param_9,uint param_10,
          undefined2 param_11)

{
  undefined2 uVar1;
  undefined4 *puVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 in_DX;
  undefined2 extraout_DX;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  undefined4 local_a;
  int local_6;
  int local_4;
  
  if (param_1 == (undefined4 *)0x0) {
    uVar1 = mem_alloc(0x2a,0x20,0);
    local_a = CONCAT22(in_DX,uVar1);
    if (local_a == 0) {
      FUN_251d_0099((char *)s_Font__Can_t_create_font_4125_3542,0x4125);
      return (undefined4 *)0x0;
    }
    puVar2 = (undefined4 *)FUN_1d28_075d(uVar1,in_DX);
    param_1 = (undefined4 *)CONCAT22(in_DX,puVar2);
    *(undefined2 *)((int)puVar2 + 0x26) = 1;
  }
  else {
    *(undefined2 *)((int)(undefined4 *)param_1 + 0x26) = 0;
  }
  uVar7 = (undefined2)((ulong)param_1 >> 0x10);
  puVar2 = (undefined4 *)param_1;
  *(uint *)((int)puVar2 + 0x26) = *(uint *)((int)puVar2 + 0x26) | param_10;
  *param_1 = param_2;
  puVar2[1] = param_3;
  puVar2[5] = param_7;
  puVar2[6] = param_8;
  *(uint *)(puVar2 + 9) = (uint)param_9;
  *(int *)(puVar2 + 4) = (int)param_5;
  *(int *)((int)puVar2 + 0x12) = (int)param_6;
  *(undefined2 *)(puVar2 + 10) = param_11;
  __F_SCOPY();
  local_4 = *param_4;
  uVar8 = (undefined2)((ulong)param_4 >> 0x10);
  local_6 = ((int *)param_4)[1];
  uVar1 = extraout_DX;
  uVar3 = mem_alloc(0xc0,0x20,0);
  uVar3 = FUN_1d28_075d(uVar3,uVar1);
  *(undefined2 *)((int)puVar2 + 0x1e) = uVar1;
  *(undefined2 *)(puVar2 + 7) = uVar3;
  mem_alloc(0xc0,0x20,0);
  uVar3 = FUN_1d28_075d();
  *(undefined2 *)((int)puVar2 + 0x22) = uVar1;
  *(undefined2 *)(puVar2 + 8) = uVar3;
  for (iVar4 = 0; iVar4 < 0x60; iVar4 = iVar4 + 1) {
    uVar1 = (undefined2)((ulong)param_7 >> 0x10);
    iVar5 = local_4 + ((*(byte *)((int)param_7 + iVar4) + 7) / 8) * 8;
    iVar6 = local_4;
    local_4 = iVar5;
    if (*param_4 + ((int *)param_4)[2] < iVar5) {
      iVar6 = *param_4;
      local_4 = (*(byte *)((int)param_7 + iVar4) + 7) / 8 << 3;
      local_6 = local_6 + (uint)param_9;
    }
    *(int *)((int)puVar2[7] + iVar4 * 2) = iVar6;
    *(int *)((int)puVar2[8] + iVar4 * 2) = local_6;
  }
  return puVar2;
}


