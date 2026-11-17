
int __cdecl16far city_at(int x,int y)

{
  int iVar1;
  
  iVar1 = 0;
  do {
    if (city_count <= iVar1) {
      return -1;
    }
    if ((*(int *)(iVar1 * 0x41 + 0x157d) == x) || (*(int *)(iVar1 * 0x41 + 0x157d) == x + -1)) {
      if (*(int *)(iVar1 * 0x41 + 0x157f) == y) {
        return iVar1;
      }
      if (*(int *)(iVar1 * 0x41 + 0x157f) == y + -1) {
        return iVar1;
      }
    }
    iVar1 = iVar1 + 1;
  } while( true );
}


