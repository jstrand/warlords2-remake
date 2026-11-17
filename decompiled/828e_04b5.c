
int __cdecl16far site_at(int x,int y)

{
  int iVar1;
  
  iVar1 = 0;
  while( true ) {
    if (site_count <= iVar1) {
      return -1;
    }
    if ((*(int *)(iVar1 * 0x1f + 0x811) == x) && (*(int *)(iVar1 * 0x1f + 0x813) == y)) break;
    iVar1 = iVar1 + 1;
  }
  return iVar1;
}


