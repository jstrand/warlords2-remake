
void __cdecl16far compute_upkeep(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  for (iVar2 = 0; iVar3 = army_count, iVar2 < 8; iVar2 = iVar2 + 1) {
    ((undefined2 *)&side_upkeep)[iVar2] = 0;
  }
  while (iVar2 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar2;
    if (((*(char *)(iVar2 * 0x16 + -0x777b) < '\b') && (-1 < *(int *)(iVar2 * 0x16 + -0x7780))) &&
       (-1 < *(int *)(iVar2 * 0x16 + -0x777e))) {
      iVar1 = (int)*(char *)(iVar2 * 0x16 + -0x7775);
      if (((*(uint *)(iVar2 * 0x16 + -0x7774) & 0x1000) != 0) && (iVar1 < 5)) {
        iVar1 = 4;
      }
      ((undefined2 *)&side_upkeep)[*(char *)(iVar2 * 0x16 + -0x777b)] =
           ((undefined2 *)&side_upkeep)[*(char *)(iVar2 * 0x16 + -0x777b)] + iVar1;
    }
  }
  return;
}


