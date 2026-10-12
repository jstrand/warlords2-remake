
int __cdecl16far FUN_828e_04b5(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = 0;
  while( true ) {
    if (site_count <= iVar1) {
      return -1;
    }
    if ((*(int *)(iVar1 * 0x1f + 0x811) == param_1) && (*(int *)(iVar1 * 0x1f + 0x813) == param_2))
    break;
    iVar1 = iVar1 + 1;
  }
  return iVar1;
}


