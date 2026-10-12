
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far save_ai_data(int param_1)

{
  void *in_DX;
  int iVar1;
  uint unaff_DI;
  
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    FUN_5db9_056c(iVar1);
    write(param_1,(void *)CONCAT22(0x42c,in_DX),unaff_DI);
    FUN_5db9_05b9(iVar1);
  }
  auto_str_no_room_ai_data();
  write(param_1,(void *)CONCAT22(0x4b0,(void *)((ulong)_DAT_451b_018a >> 0x10)),unaff_DI);
  FUN_5db9_063d();
  return;
}


