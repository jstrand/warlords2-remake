
void __cdecl16far FUN_7563_0bd9(void)

{
  int iVar1;
  undefined1 local_6 [4];
  
  iVar1 = FUN_7563_0b47(current_player,local_6);
  if (iVar1 == 0) {
    for (iVar1 = 0; iVar1 < city_count; iVar1 = iVar1 + 1) {
      if (((*(char *)(iVar1 * 0x41 + 0x1592) == current_player) &&
          (*(int *)((undefined1 *)&LAB_4125_15ad_1 + iVar1 * 0x41) != 0)) &&
         (*(int *)((undefined1 *)&LAB_4125_15b0 + iVar1 * 0x41) == -2)) {
        *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + iVar1 * 0x41) = 0;
      }
    }
  }
  return;
}


