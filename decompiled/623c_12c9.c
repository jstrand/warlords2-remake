
int __cdecl16far FUN_623c_12c9(int param_1,int param_2,long param_3,int param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_SS;
  int local_12 [8];
  
  iVar2 = 8;
  while (iVar2 != 0) {
    local_12[iVar2 + -1] = -1;
    iVar2 = iVar2 + -1;
  }
  iVar3 = 0;
  iVar2 = army_count;
  do {
    do {
      iVar1 = iVar2 + -1;
      if (iVar2 == 0) goto LAB_623c_1370;
      iVar2 = iVar1;
    } while ((((*(char *)(iVar1 * 0x16 + -0x777b) != (char)current_player) ||
              (*(int *)(iVar1 * 0x16 + -0x7780) != param_1)) ||
             (*(int *)(iVar1 * 0x16 + -0x777e) != param_2)) ||
            ((param_4 != 0 && (*(char *)(iVar1 * 0x16 + -0x7779) < param_4))));
    local_12[iVar3] = iVar1;
    iVar3 = iVar3 + 1;
  } while (iVar3 < 8);
LAB_623c_1370:
  if (iVar3 != 0) {
    FUN_623c_14d3(local_12);
  }
  if (param_3 != 0) {
    iVar2 = 8;
    while (iVar1 = iVar2 + -1, iVar2 != 0) {
      *(int *)((int)param_3 + iVar1 * 2) = local_12[iVar1];
      iVar2 = iVar1;
    }
  }
  return iVar3;
}


