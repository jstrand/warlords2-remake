
undefined2 __cdecl16far path_single_step(int *param_1)

{
  char cVar1;
  bool bVar2;
  bool bVar3;
  int iVar4;
  int *piVar5;
  undefined2 uVar6;
  
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  piVar5 = (int *)param_1;
  cVar1 = *(char *)(*(byte *)(piVar5[1] * 0xe0 + *param_1 * 2) + 0x710);
  iVar4 = (int)*(char *)(*(byte *)(piVar5[3] * 0xe0 + piVar5[2] * 2) + 0x710);
  if ((cVar1 == '\x02') || (cVar1 == '\x03')) {
    bVar2 = true;
  }
  else {
    bVar2 = false;
  }
  if ((iVar4 == 2) || (iVar4 == 3)) {
    bVar3 = true;
  }
  else {
    bVar3 = false;
  }
  if (piVar5[5] != 2) {
    if (((bVar2) && (!bVar3)) || ((!bVar2 && (bVar3)))) {
      return 0;
    }
    if (*(int *)((undefined1 *)&terrain_move_cost + iVar4 * 2) == 0) {
      return 0;
    }
  }
  iVar4 = direction_between(*param_1,piVar5[1],piVar5[2],piVar5[3]);
  **(undefined1 **)(piVar5 + 6) = (char)iVar4;
  return 1;
}


