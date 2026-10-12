
int __cdecl16far FUN_1b62_0044(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = army_count;
  do {
    iVar2 = iVar1 + -1;
    if (iVar1 == 0) {
      return 0;
    }
    iVar1 = iVar2;
  } while ((param_2 != 0x2e1d) || (iVar2 * 0x16 + -0x7780 != param_1));
  return iVar2;
}


