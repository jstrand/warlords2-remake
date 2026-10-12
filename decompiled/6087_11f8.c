
void __cdecl16far FUN_6087_11f8(int param_1,undefined4 param_2)

{
  int iVar1;
  int y1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  undefined2 uVar10;
  int local_a;
  uint local_8;
  
  uVar10 = (undefined2)((ulong)param_2 >> 0x10);
  iVar7 = (int)param_2;
  iVar1 = *(int *)(iVar7 + param_1 * 2);
  local_8 = 0xffff;
  local_a = 100;
  if ((*(byte *)(iVar1 * 0x16 + -0x7772) & 0xf) == 1) {
    iVar3 = 5;
  }
  else {
    iVar3 = 0xf;
  }
  uVar8 = 0x16;
  while (uVar9 = uVar8 - 1, uVar8 != 0) {
    iVar5 = *(int *)(uVar9 * 0x1d + 0xd02);
    y1 = *(int *)(uVar9 * 0x1d + 0xd04);
    bVar2 = true;
    iVar4 = FUN_6087_1736(uVar9);
    uVar8 = uVar9;
    if (iVar4 != 0) {
      iVar4 = 6;
      while (iVar6 = iVar4 + -1, iVar4 != 0) {
        iVar4 = iVar6;
        if ((((iVar6 != param_1) && (*(int *)(iVar7 + iVar6 * 2) != -1)) &&
            ((*(byte *)(*(int *)(iVar7 + iVar6 * 2) * 0x16 + -0x7772) & 0xf) == 2)) &&
           (*(byte *)(*(int *)(iVar7 + iVar6 * 2) * 0x16 + -0x7771) >> 1 == uVar9)) {
          bVar2 = false;
        }
      }
      if (((bVar2) &&
          (iVar5 = map_distance(iVar5,y1,*(int *)(iVar1 * 0x16 + -0x7780),
                                *(int *)(iVar1 * 0x16 + -0x777e)), iVar5 <= iVar3)) &&
         (iVar5 < local_a)) {
        local_a = iVar5;
        local_8 = uVar9;
      }
    }
  }
  *(uint *)(iVar7 + 0x3c) = local_8;
  *(int *)(iVar7 + 0x3e) = local_a;
  return;
}


