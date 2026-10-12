
int __cdecl16far ai_collect_stack(int x,int y,int *out,int order_hi,int order_lo,int min_moves)

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
      if (iVar2 == 0) goto LAB_623c_1491;
      iVar2 = iVar1;
    } while ((((*(char *)(iVar1 * 0x16 + -0x777b) != (char)current_player) ||
              (*(int *)(iVar1 * 0x16 + -0x7780) != x)) || (*(int *)(iVar1 * 0x16 + -0x777e) != y))
            || ((((*(byte *)(iVar1 * 0x16 + -0x7772) & 0x70) >> 4 != order_hi ||
                 ((*(byte *)(iVar1 * 0x16 + -0x7772) & 0xf) != order_lo)) ||
                ((min_moves != 0 && (*(char *)(iVar1 * 0x16 + -0x777a) < min_moves))))));
    local_12[iVar3] = iVar1;
    iVar3 = iVar3 + 1;
  } while (iVar3 < 8);
LAB_623c_1491:
  if (iVar3 != 0) {
    FUN_623c_14d3(local_12);
  }
  if (out != (int *)0x0) {
    iVar2 = 8;
    while (iVar1 = iVar2 + -1, iVar2 != 0) {
      ((int *)out)[iVar1] = local_12[iVar1];
      iVar2 = iVar1;
    }
  }
  return iVar3;
}


