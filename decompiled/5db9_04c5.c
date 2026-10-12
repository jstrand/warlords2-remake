
void __cdecl16far auto_str_no_room_ai_memory(int param_1)

{
  int iVar1;
  int in_DX;
  int iVar2;
  int iVar3;
  
  DAT_4125_0832 = 1;
  iVar2 = 8;
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    iVar1 = mem_alloc(0x42c,0,0);
    *(int *)(iVar3 * 4 + 0x40cc) = in_DX;
    *(int *)(iVar3 * 4 + 0x40ca) = iVar1;
    iVar2 = iVar3;
    if (iVar1 == 0 && in_DX == 0) {
      auto_str_fatal_error(0x909,0x4125);
    }
  }
  if (param_1 != 0) {
    FUN_5db9_0519();
  }
  return;
}


