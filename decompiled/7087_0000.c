
int __cdecl16far FUN_7087_0000(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = 0;
  iVar2 = 0;
  while ((iVar1 < 4 && (*(char *)(param_1 * 0x41 + iVar1 + 0x1593) != param_2))) {
    iVar2 = iVar2 + 1;
    iVar1 = iVar1 + 1;
  }
  if (3 < iVar2) {
    iVar2 = 0;
  }
  return iVar2;
}


