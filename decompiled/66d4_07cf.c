
int __cdecl16far side_gold_per_city(int side)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  iVar1 = army_count;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if ((*(char *)(iVar2 * 0x16 + -0x777b) == side) && (*(char *)(iVar2 * 0x16 + -0x777c) == '\x1c')
       ) {
      list_carried_items(2,0,iVar2 * 0x16 + -0x7780,0x2e1d);
      for (iVar2 = 0; iVar2 < DAT_451b_08ce; iVar2 = iVar2 + 1) {
        if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[iVar2] * 0x1d + 0xcfd) == '\a') {
          iVar3 = iVar3 + *(char *)(((undefined2 *)&DAT_451b_0c9c)[iVar2] * 0x1d + 0xcfe);
        }
      }
    }
  }
  return iVar3;
}


