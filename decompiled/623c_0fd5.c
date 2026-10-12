
undefined2 __cdecl16far city_is_building(int param_1)

{
  int iVar1;
  
  if ((*(char *)(param_1 * 0x41 + 0x15aa) != '\0') &&
     (iVar1 = FUN_7087_0000(param_1,(int)*(char *)(param_1 * 0x41 + 0x15a9)),
     *(char *)(param_1 * 0x41 + 0x15aa) != *(char *)(param_1 * 0x41 + iVar1 + 0x1597))) {
    return 1;
  }
  return 0;
}


