
void __cdecl16far FUN_623c_1626(int param_1,int param_2,int *param_3,int *param_4,int *param_5)

{
  int iVar1;
  int iVar2;
  
  *param_5 = 0;
  *param_4 = 0;
  *param_3 = 0;
  iVar1 = army_count;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if ((*(int *)(iVar2 * 0x16 + -0x7780) == param_1) &&
       (*(int *)(iVar2 * 0x16 + -0x777e) == param_2)) {
      *param_3 = *param_3 + 1;
      *param_4 = *param_4 + (int)*(char *)(iVar2 * 0x16 + -0x7778);
      if (*(char *)(iVar2 * 0x16 + -0x777c) == '\x1c') {
        *param_5 = *param_5 + 1;
      }
    }
  }
  return;
}


