
void __cdecl16far FUN_828e_03f9(void)

{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    ((undefined2 *)&side_city_count)[iVar1] = 0;
  }
  for (iVar1 = 0; iVar1 < city_count; iVar1 = iVar1 + 1) {
    if (*(char *)(iVar1 * 0x41 + 0x1592) < '\b') {
      ((undefined2 *)&side_city_count)[*(char *)(iVar1 * 0x41 + 0x1592)] =
           ((undefined2 *)&side_city_count)[*(char *)(iVar1 * 0x41 + 0x1592)] + 1;
    }
  }
  return;
}


