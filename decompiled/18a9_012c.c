
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_18a9_012c(int *param_1)

{
  int iVar1;
  
  iVar1 = FUN_18a9_0756();
  if (iVar1 != 0) {
    if (*param_1 == 3) {
      if (DAT_4125_166c == 0) {
        auto_file_warlord2_hlp(*(undefined2 *)((int)*DAT_451b_0ce0 + iVar1 * 0x21));
      }
    }
    else if (((_LAB_4125_1174 == 0) && (_LAB_4125_1176 == 0)) && (_LAB_4125_1172 == 0)) {
      DAT_451b_0cde = iVar1;
      FUN_17be_0e26(*param_1);
    }
  }
  return;
}


