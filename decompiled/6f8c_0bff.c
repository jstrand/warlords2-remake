
bool __cdecl16far FUN_6f8c_0bff(int param_1,int param_2,int *param_3,int *param_4)

{
  int iVar1;
  undefined2 unaff_SS;
  bool bVar2;
  int iStack_c;
  int local_a [4];
  
  bVar2 = false;
  iVar1 = 0;
  while( true ) {
    if (3 < iVar1) {
      if (param_2 != 0) {
        for (iStack_c = 0; (bVar2 == false && (iStack_c < 0x14)); iStack_c = iStack_c + 1) {
          iVar1 = dice(1,3,-2);
          *param_3 = *(int *)(param_1 * 0x41 + 0x157d) + iVar1;
          iVar1 = dice(1,3,-2);
          *param_4 = *(int *)(param_1 * 0x41 + 0x157f) + iVar1;
          bVar2 = (*(byte *)(*param_4 * 0xe0 + *param_3 * 2 + 1) & 0x10) == 0;
        }
      }
      return bVar2;
    }
    armies_at_tile(*(int *)(param_1 * 0x41 + 0x157d) + *(int *)(iVar1 * 2 + 0xe54),
                   *(int *)(param_1 * 0x41 + 0x157f) + *(int *)(iVar1 * 2 + 0xe5c),
                   (int *)CONCAT22(unaff_SS,local_a + iVar1));
    if (local_a[iVar1] < 8) break;
    iVar1 = iVar1 + 1;
  }
  *param_3 = *(int *)(param_1 * 0x41 + 0x157d) + *(int *)(iVar1 * 2 + 0xe54);
  *param_4 = *(int *)(param_1 * 0x41 + 0x157f) + *(int *)(iVar1 * 2 + 0xe5c);
  return true;
}


