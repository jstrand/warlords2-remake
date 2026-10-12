
int __cdecl16far armies_at_tile(int x,int y,int *count)

{
  int iVar1;
  int iVar2;
  int iVar3;
  char cVar4;
  undefined4 local_6;
  
  local_6 = 0;
  cVar4 = '\0';
  *count = 0;
  iVar2 = 8;
  while (iVar3 = iVar2 + -1, iVar1 = army_count, iVar2 != 0) {
    ((undefined2 *)&DAT_451b_1ec0)[iVar3 * 2] = 0;
    ((undefined2 *)&DAT_451b_1ebe)[iVar3 * 2] = 0;
    iVar2 = iVar3;
  }
  while( true ) {
    iVar2 = iVar1 + -1;
    if (iVar1 == 0) break;
    iVar1 = iVar2;
    if (((*(int *)(iVar2 * 0x16 + -0x7780) == x) && (*(int *)(iVar2 * 0x16 + -0x777e) == y)) &&
       (*count < 8)) {
      iVar3 = *count;
      ((undefined2 *)&DAT_451b_1ec0)[iVar3 * 2] = 0x2e1d;
      ((undefined2 *)&DAT_451b_1ebe)[iVar3 * 2] = iVar2 * 0x16 + -0x7780;
      iVar3 = (int)*(char *)(iVar2 * 0x16 + -0x777b);
      if (iVar3 == 0xf) {
        iVar3 = 8;
      }
      if ((local_6 == 0) ||
         (cVar4 < *(char *)(iVar3 * 0x1d + (int)*(char *)((int)local_6 + 4) + 0x60b))) {
        local_6 = CONCAT22(0x2e1d,iVar2 * 0x16 + -0x7780);
        cVar4 = *(char *)(iVar3 * 0x1d + (int)*(char *)(iVar2 * 0x16 + -0x777c) + 0x60b);
      }
      *count = *count + 1;
    }
  }
  return (int)local_6;
}


