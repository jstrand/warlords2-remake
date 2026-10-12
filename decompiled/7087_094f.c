
int __cdecl16far city_production_count(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = 0;
  for (iVar2 = 0; iVar2 < 4; iVar2 = iVar2 + 1) {
    if (-1 < *(char *)(param_1 * 0x41 + iVar2 + 0x1593)) {
      iVar1 = iVar1 + 1;
    }
  }
  return iVar1;
}


