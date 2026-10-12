
void __cdecl16far
FUN_1997_027e(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

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
  if ((((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 0x20) == 0) {
    FUN_24d0_009d(*(undefined4 *)(param_1 * 0xc + 0x1f57),&param_2);
  }
  else {
    (*DAT_451b_2a68)(0x1997,*(undefined4 *)(param_1 * 0xc + 0x1f57),param_2,param_3,0x342a,0x4125,
                     param_4,*(undefined4 *)(param_1 * 0xc + 0x1f5c),param_2);
  }
  return;
}


