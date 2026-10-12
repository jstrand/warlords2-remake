
void __cdecl16far hero_drop_items(undefined4 param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_SS;
  undefined1 local_f;
  int local_c;
  int local_a [4];
  
  local_f = 1;
  *(undefined2 *)(*(char *)((int)param_1 + 10) * 2 + 0x543) = 0;
  list_carried_items(2,0,param_1);
  if ((0 < DAT_451b_08ce) && (*(char *)(*(byte *)(param_3 * 0xe0 + param_2 * 2) + 0x710) == '\x02'))
  {
    sound_splash();
    local_f = 0;
  }
  for (iVar2 = 0; iVar2 < DAT_451b_08ce; iVar2 = iVar2 + 1) {
    *(undefined1 *)(((undefined2 *)&DAT_451b_0c9c)[iVar2] * 0x1d + 0xcff) = local_f;
    *(undefined2 *)(((undefined2 *)&DAT_451b_0c9c)[iVar2] * 0x1d + 0xd00) = 0;
    *(int *)(((undefined2 *)&DAT_451b_0c9c)[iVar2] * 0x1d + 0xd02) = param_2;
    *(int *)(((undefined2 *)&DAT_451b_0c9c)[iVar2] * 0x1d + 0xd04) = param_3;
    if (((undefined2 *)&DAT_451b_0c9c)[iVar2] == current_player) {
      FUN_7087_0410(0xfffe,local_a);
      for (iVar1 = 0; iVar1 < local_c; iVar1 = iVar1 + 1) {
        *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + local_a[iVar1] * 0x41) = 0;
        *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + local_a[iVar1] * 0x41) = 0xffff;
        *(undefined2 *)(local_a[iVar1] * 0x41 + 0x15b2) = 0xffff;
      }
    }
  }
  return;
}


