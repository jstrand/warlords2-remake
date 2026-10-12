
undefined2 __cdecl16far FUN_5db9_056c(int param_1)

{
  undefined2 uVar1;
  undefined2 in_DX;
  
  if (DAT_4125_0832 == 0) {
    auto_str_no_room_ai_memory();
  }
  if ((DAT_4125_0836 == 0) || (uVar1 = (undefined2)DAT_451b_018e, current_player != param_1)) {
    DAT_451b_0162 = mem_lock(*(undefined4 *)(param_1 * 4 + 0x40ca));
    uVar1 = DAT_451b_0162;
    DAT_451b_0164 = in_DX;
  }
  return uVar1;
}


