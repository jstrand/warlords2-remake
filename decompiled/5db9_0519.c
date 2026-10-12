
void __cdecl16far FUN_5db9_0519(void)

{
  undefined2 in_DX;
  int iVar1;
  int iVar2;
  
  if (DAT_4125_0832 == 0) {
    auto_str_no_room_ai_memory();
  }
  iVar1 = 8;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    DAT_451b_018e._0_2_ = mem_lock(*(undefined4 *)(iVar2 * 4 + 0x40ca));
    DAT_451b_018e._2_2_ = in_DX;
    ai_data_init();
    mem_unlock(*(undefined4 *)(iVar2 * 4 + 0x40ca));
    iVar1 = iVar2;
  }
  return;
}


