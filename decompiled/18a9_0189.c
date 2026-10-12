
void __cdecl16far FUN_18a9_0189(int *param_1)

{
  int iVar1;
  
  if (*param_1 != 4) {
    if (DAT_4125_166c != 0) {
      iVar1 = FUN_18a9_02f1();
      if (iVar1 != 0) {
        FUN_17be_04d8(0x18a9,*(undefined2 *)((int)*DAT_451b_0ce0 + DAT_451b_0cdc * 0x21),*param_1);
      }
    }
    DAT_4125_166c = 0;
  }
  return;
}


