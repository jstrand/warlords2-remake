
void __cdecl16far FUN_7087_0410(int param_1,undefined4 param_2,int *param_3)

{
  int iVar1;
  undefined2 uVar2;
  
  *param_3 = 0;
  uVar2 = (undefined2)((ulong)param_2 >> 0x10);
  if ((param_1 == -2) || (99 < param_1)) {
    for (iVar1 = 0; iVar1 < city_count; iVar1 = iVar1 + 1) {
      if (((*(int *)((undefined1 *)&LAB_4125_15ad_1 + iVar1 * 0x41) != 0) &&
          (*(char *)(iVar1 * 0x41 + 0x1592) == current_player)) &&
         (*(int *)((undefined1 *)&LAB_4125_15b0 + iVar1 * 0x41) == -2)) {
        *(int *)((int)param_2 + *param_3 * 2) = iVar1;
        *param_3 = *param_3 + 1;
      }
    }
  }
  else {
    for (iVar1 = 0; iVar1 < city_count; iVar1 = iVar1 + 1) {
      if (((*(int *)((undefined1 *)&LAB_4125_15ad_1 + iVar1 * 0x41) != 0) &&
          (*(int *)((undefined1 *)&LAB_4125_15b0 + iVar1 * 0x41) ==
           *(int *)(param_1 * 0x41 + 0x157d))) &&
         (*(int *)(iVar1 * 0x41 + 0x15b2) == *(int *)(param_1 * 0x41 + 0x157f))) {
        *(int *)((int)param_2 + *param_3 * 2) = iVar1;
        *param_3 = *param_3 + 1;
      }
    }
  }
  return;
}


