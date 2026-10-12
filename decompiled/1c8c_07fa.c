
undefined2 __cdecl16far FUN_1c8c_07fa(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined4 uVar1;
  bool bVar2;
  bool bVar3;
  bool bVar4;
  bool bVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int *piVar9;
  int iVar10;
  undefined2 uVar11;
  undefined2 local_4;
  
  local_4 = 0;
  bVar3 = true;
  bVar4 = true;
  bVar5 = false;
  bVar2 = false;
  for (iVar10 = 0; iVar10 < param_3; iVar10 = iVar10 + 1) {
    uVar11 = (undefined2)((ulong)param_1 >> 0x10);
    iVar8 = (int)param_1;
    piVar9 = (int *)(iVar8 + iVar10 * 4);
    if ((*piVar9 != 0 || piVar9[1] != 0) && (*(int *)((int)param_2 + iVar10 * 2) != 0)) {
      uVar1 = *(undefined4 *)(iVar8 + iVar10 * 4);
      iVar6 = (int)*(char *)((int)uVar1 + 4);
      uVar1 = *(undefined4 *)(iVar8 + iVar10 * 4);
      uVar7 = *(uint *)((int)uVar1 + 0xc) & 0x1000;
      if ((iVar6 == 0x1c) &&
         (iVar8 = hero_flight_items(*(undefined4 *)(iVar8 + iVar10 * 4)), iVar8 != 0)) {
        bVar2 = true;
      }
      if ((((undefined1 *)&army_move_flags)[iVar6 * 6] == '\0') || (uVar7 != 0)) {
        bVar3 = false;
      }
      if ((iVar6 != 0x1c) && (((undefined1 *)&army_move_flags)[iVar6 * 6] == '\0')) {
        bVar4 = false;
      }
      if ((((undefined1 *)&army_move_flags)[iVar6 * 6] != '\0') && (uVar7 == 0)) {
        bVar5 = true;
      }
    }
  }
  if (((bVar2) || (bVar3)) || ((bVar4 && (bVar5)))) {
    local_4 = 1;
  }
  return local_4;
}


