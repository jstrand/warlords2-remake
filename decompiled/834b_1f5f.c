
void __cdecl16far FUN_834b_1f5f(int param_1,int param_2,int param_3)

{
  int iVar1;
  
  param_2 = param_2 * 2 + -6;
  iVar1 = (param_1 * 2 + -2 >> 3) << 3;
  if (iVar1 < 0) {
    iVar1 = 0;
  }
  if (param_2 < 0) {
    param_2 = 0;
  }
  FUN_1997_034a(9);
  (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,*(undefined2 *)(param_3 * 8 + 0x2cae),
                   *(undefined2 *)(param_3 * 8 + 0x2cb0),*(undefined2 *)(param_3 * 8 + 0x2cb2),
                   *(undefined2 *)(param_3 * 8 + 0x2cb4),0x6795,0x4125,iVar1,param_2,0x576c,0x4125,
                   *(undefined2 *)(param_3 * 8 + 0x2cae),*(undefined2 *)(param_3 * 8 + 0x2cb0));
  return;
}


