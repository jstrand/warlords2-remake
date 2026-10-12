
uint __cdecl16far FUN_216d_02d8(int *param_1)

{
  byte bVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  int *piVar10;
  int iVar11;
  undefined1 *puVar12;
  undefined2 uVar13;
  undefined2 uVar14;
  int local_12;
  byte local_10;
  undefined1 local_e;
  undefined4 local_6;
  
  uVar13 = (undefined2)((ulong)param_1 >> 0x10);
  piVar10 = (int *)param_1;
  uVar14 = (undefined2)((ulong)DAT_451b_30e9 >> 0x10);
  iVar11 = (int)DAT_451b_30e9;
  iVar3 = *param_1 + *(int *)(iVar11 + 8);
  uVar2 = *(undefined2 *)(iVar11 + 0x5d);
  local_12 = 0;
  if (((*DAT_4125_34e7 == 1) || (*DAT_4125_34e7 == 2)) || (*DAT_4125_34e7 == 4)) {
    iVar4 = 0x50;
  }
  else {
    iVar4 = 0x28;
  }
  local_10 = *(byte *)(iVar3 % 8 + 0x352e);
  iVar5 = 8 - iVar3 % 8;
  iVar9 = (iVar3 + piVar10[2] + -1) % 8;
  bVar1 = *(byte *)(iVar9 + 0x3536);
  uVar8 = (uint)bVar1;
  iVar9 = iVar9 + 1;
  if (iVar5 < piVar10[2]) {
    if (piVar10[2] != iVar5 + iVar9) {
      local_12 = ((piVar10[2] - iVar5) - iVar9) / 8;
    }
  }
  else {
    local_10 = local_10 & bVar1;
    uVar8 = 0xffff;
  }
  out(0x3ce,5);
  out(0x3cf,2);
  out(0x3c4,2);
  out(0x3c5,0xf);
  out(0x3ce,8);
  uVar6 = (piVar10[1] + *(int *)(iVar11 + 10)) * iVar4 + (iVar3 >> 3);
  uVar7 = uVar6;
  for (iVar3 = 0; iVar3 < piVar10[3]; iVar3 = iVar3 + 1) {
    puVar12 = (undefined1 *)(uVar6 + iVar3 * iVar4);
    local_6 = (undefined1 *)CONCAT22(0xa000,puVar12);
    out(0x3cf,local_10);
    local_e = (undefined1)uVar2;
    *local_6 = local_e;
    local_6 = (undefined1 *)CONCAT22(0xa000,puVar12 + 1);
    out(0x3cf,0xff);
    iVar11 = local_12;
    while (iVar11 != 0) {
      *local_6 = local_e;
      local_6 = (undefined1 *)CONCAT22(local_6._2_2_,(undefined1 *)local_6 + 1);
      iVar11 = iVar11 + -1;
    }
    if (uVar8 != 0) {
      out(0x3cf,(char)uVar8);
      *local_6 = local_e;
    }
    uVar7 = 0;
  }
  out(0x3ce,8);
  out(0x3cf,0xff);
  out(0x3ce,5);
  out(0x3cf,0);
  return uVar7 & 0xff00;
}


