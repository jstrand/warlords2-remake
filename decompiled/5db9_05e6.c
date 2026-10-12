
void __cdecl16far auto_str_no_room_ai_data(void)

{
  int iVar1;
  int in_DX;
  
  if (DAT_4125_0834 == 0) {
    iVar1 = mem_alloc(0x4b0,0,0);
    DAT_451b_0166 = CONCAT22(in_DX,iVar1);
    if (iVar1 == 0 && in_DX == 0) {
      auto_str_fatal_error(0x91b,0x4125);
    }
    DAT_4125_0834 = 1;
  }
  if (DAT_4125_0836 == 0) {
    DAT_451b_018a = mem_lock(DAT_451b_0166);
    DAT_451b_018c = in_DX;
  }
  return;
}


