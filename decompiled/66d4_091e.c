
void __cdecl16far mark_rich_sites(void)

{
  bool bVar1;
  int iVar2;
  int in_CX;
  int iVar3;
  
  iVar2 = site_count * 3;
  for (iVar3 = 0; iVar3 < site_count; iVar3 = iVar3 + 1) {
    *(undefined2 *)(iVar3 * 0x1f + 0x82c) = 0;
    *(undefined2 *)(iVar3 * 0x1f + 0x82e) = 0xff;
  }
  for (iVar3 = 0; iVar3 < iVar2 / 10; iVar3 = iVar3 + 1) {
    bVar1 = false;
    while (!bVar1) {
      in_CX = dice(1,site_count,-1);
      if ((*(char *)(in_CX * 0x1f + 0x829) != '\x01') && (*(int *)(in_CX * 0x1f + 0x82c) == 0)) {
        bVar1 = true;
      }
    }
    *(undefined2 *)(in_CX * 0x1f + 0x82c) = 1;
    if (opt_quests != 0) {
      *(undefined2 *)(in_CX * 0x1f + 0x82e) = 0;
    }
  }
  return;
}


