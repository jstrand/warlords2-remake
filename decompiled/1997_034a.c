
void __cdecl16far FUN_1997_034a(int param_1)

{
  int iVar1;
  
  if ((((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 4) == 0) {
    for (iVar1 = 0; iVar1 < DAT_4125_230a; iVar1 = iVar1 + 1) {
      if (((((undefined1 *)&DAT_4125_1f56)[iVar1 * 0xc] & 4) != 0) &&
         (((undefined1 *)&DAT_4125_1f5b)[iVar1 * 0xc] ==
          ((undefined1 *)&DAT_4125_1f5b)[param_1 * 0xc])) {
        FUN_1997_059c(iVar1);
        break;
      }
    }
    auto_file_files_any_pck(param_1);
  }
  return;
}


