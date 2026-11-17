
int __cdecl16far city_free_tile(int city,int spill,int *x,int *y)

{
  int iVar1;
  undefined2 unaff_SS;
  uint local_e;
  int iStack_c;
  int local_a [4];
  
  local_e = 0;
  iVar1 = 0;
  while( true ) {
    if (3 < iVar1) {
      if (spill != 0) {
        for (iStack_c = 0; (local_e == 0 && (iStack_c < 0x14)); iStack_c = iStack_c + 1) {
          iVar1 = dice(1,3,-2);
          *x = *(int *)(city * 0x41 + 0x157d) + iVar1;
          iVar1 = dice(1,3,-2);
          *y = *(int *)(city * 0x41 + 0x157f) + iVar1;
          local_e = (uint)((*(byte *)(*y * 0xe0 + *x * 2 + 1) & 0x10) == 0);
        }
      }
      return local_e;
    }
    armies_at_tile(*(int *)(city * 0x41 + 0x157d) + *(int *)(iVar1 * 2 + 0xe54),
                   *(int *)(city * 0x41 + 0x157f) + *(int *)(iVar1 * 2 + 0xe5c),
                   (int *)CONCAT22(unaff_SS,local_a + iVar1));
    if (local_a[iVar1] < 8) break;
    iVar1 = iVar1 + 1;
  }
  *x = *(int *)(city * 0x41 + 0x157d) + *(int *)(iVar1 * 2 + 0xe54);
  *y = *(int *)(city * 0x41 + 0x157f) + *(int *)(iVar1 * 2 + 0xe5c);
  return 1;
}


