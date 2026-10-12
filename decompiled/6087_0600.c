
int __cdecl16far FUN_6087_0600(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = 0;
  iVar2 = 0;
  while( true ) {
    if (site_count <= iVar2) {
      return -1;
    }
    if (param_1 == iVar2) break;
    if ((*(char *)(iVar2 * 0x1f + 0x829) == '\x01') && (iVar1 < 4)) {
      iVar1 = iVar1 + 1;
    }
    iVar2 = iVar2 + 1;
  }
  return iVar1;
}


