
void __cdecl16far
FUN_1997_00a2(int param_1,undefined4 param_2,undefined4 param_3,int param_4,undefined4 param_5)

{
  int iVar1;
  
  if ((((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 4) == 0) {
    for (iVar1 = 0; iVar1 < DAT_4125_230a; iVar1 = iVar1 + 1) {
      if (((((undefined1 *)&DAT_4125_1f56)[iVar1 * 0xc] & 4) != 0) &&
         (((undefined1 *)&DAT_4125_1f5b)[iVar1 * 0xc] ==
          ((undefined1 *)&DAT_4125_1f5b)[param_1 * 0xc])) {
        FUN_1997_059c();
        break;
      }
    }
    auto_file_files_any_pck();
  }
  (*DAT_451b_2a74)(0x1997,*(undefined4 *)(param_1 * 0xc + 0x1f57),param_2,param_3,
                   *(undefined4 *)(param_4 * 0xc + 0x1f57),param_5);
  return;
}


