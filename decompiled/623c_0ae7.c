
undefined2 __cdecl16far
FUN_623c_0ae7(undefined4 param_1,int param_2,undefined2 param_3,uint param_4)

{
  byte *pbVar1;
  uint *puVar2;
  char cVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 in_DX;
  int iVar6;
  int iVar7;
  undefined1 local_a;
  
  cVar3 = -1;
  iVar4 = FUN_1b62_0024(current_player);
  local_a = 0;
  iVar6 = 0;
  for (iVar7 = 0; uVar5 = (undefined2)((ulong)param_1 >> 0x10), iVar7 < 8; iVar7 = iVar7 + 1) {
    if (*(int *)((int)param_1 + iVar7 * 2) != -1) {
      iVar6 = iVar6 + 1;
    }
  }
  if (iVar6 == 0) {
    uVar5 = 0;
  }
  else {
    if (1 < iVar6) {
      local_a = FUN_1b62_0cde();
    }
    for (iVar6 = 0; iVar6 < 8; iVar6 = iVar6 + 1) {
      iVar7 = *(int *)((int)param_1 + iVar6 * 2);
      if (iVar7 == -1) {
        *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar6 * 4) = 0;
        ((undefined2 *)&DAT_451b_1ede)[iVar6 * 2] = 0;
      }
      else {
        *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar6 * 4) = 0x2e1d;
        ((undefined2 *)&DAT_451b_1ede)[iVar6 * 2] = iVar7 * 0x16 + -0x7780;
        if (cVar3 < *(char *)(iVar4 + *(char *)((int)*(undefined4 *)
                                                      ((undefined2 *)&DAT_451b_1ede + iVar6 * 2) + 4
                                               ))) {
          cVar3 = *(char *)(iVar4 + *(char *)((int)*(undefined4 *)
                                                    ((undefined2 *)&DAT_451b_1ede + iVar6 * 2) + 4))
          ;
          DAT_451b_1f02._0_2_ = iVar7 * 0x16 + -0x7780;
          DAT_451b_1f02._2_2_ = 0x2e1d;
        }
        if (param_2 != -1) {
          pbVar1 = (byte *)(iVar7 * 0x16 + -0x7772);
          *pbVar1 = *pbVar1 & 0xf0;
          pbVar1 = (byte *)(iVar7 * 0x16 + -0x7772);
          *pbVar1 = *pbVar1 | (byte)param_2 & 0xf;
          pbVar1 = (byte *)(iVar7 * 0x16 + -0x7771);
          *pbVar1 = *pbVar1 & 1;
          pbVar1 = (byte *)(iVar7 * 0x16 + -0x7771);
          *pbVar1 = *pbVar1 | (char)param_3 << 1;
          *(undefined1 *)(iVar7 * 0x16 + -0x776f) = local_a;
          FUN_623c_0c7e(iVar7 * 0x16 + -0x7780,0x2e1d,param_2,param_3);
        }
        puVar2 = (uint *)(iVar7 * 0x16 + -0x7774);
        *puVar2 = *puVar2 & 0xff5f;
        if (param_4 != 0) {
          puVar2 = (uint *)(iVar7 * 0x16 + -0x7774);
          *puVar2 = *puVar2 | param_4;
        }
      }
    }
    stack_movement_mode();
    uVar5 = 1;
  }
  return uVar5;
}


