
int __cdecl16far FUN_828e_044a(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = 0;
  do {
    if (city_count <= iVar1) {
      return -1;
    }
    if ((*(int *)(iVar1 * 0x41 + 0x157d) == param_1) ||
       (*(int *)(iVar1 * 0x41 + 0x157d) == param_1 + -1)) {
      if (*(int *)(iVar1 * 0x41 + 0x157f) == param_2) {
        return iVar1;
      }
      if (*(int *)(iVar1 * 0x41 + 0x157f) == param_2 + -1) {
        return iVar1;
      }
    }
    iVar1 = iVar1 + 1;
  } while( true );
}


