
void __cdecl16far FUN_7bab_2051(void)

{
  int iVar1;
  int iVar2;
  
  for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
    if (((*(int *)(iVar2 * 2 + 0x137) != 0) && (*(int *)(iVar2 * 2 + 0xd0) == 1)) &&
       (*(int *)(iVar2 * 2 + 0xc0) != 3)) {
      list_carried_items(7,*(undefined2 *)(iVar2 * 2 + 0xc0),0xffff,0);
      iVar1 = dice(1,DAT_451b_08ce + -1,0);
      *(int *)(iVar2 * 2 + 0xe0) = iVar1;
      FUN_7bab_0634(iVar2,0);
    }
  }
  FUN_7bab_0416();
  return;
}


