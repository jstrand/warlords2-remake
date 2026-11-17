
int __cdecl16far nearest_city(int x,int y,int side,int seen_only)

{
  int iVar1;
  int iVar2;
  int local_6;
  int local_4;
  
  local_4 = -1;
  local_6 = 1000;
  for (iVar2 = 0; iVar2 < city_count; iVar2 = iVar2 + 1) {
    if ((((((opt_hidden_map == 0) || (seen_only == 0)) ||
          (iVar1 = FUN_8611_15ff(1,*(undefined2 *)(iVar2 * 0x41 + 0x157d),
                                 *(undefined2 *)(iVar2 * 0x41 + 0x157f)), iVar1 != 0)) ||
         ((iVar1 = FUN_8611_15ff(1,*(int *)(iVar2 * 0x41 + 0x157d) + 1,
                                 *(undefined2 *)(iVar2 * 0x41 + 0x157f)), iVar1 != 0 ||
          (iVar1 = FUN_8611_15ff(1,*(undefined2 *)(iVar2 * 0x41 + 0x157d),
                                 *(int *)(iVar2 * 0x41 + 0x157f) + 1), iVar1 != 0)))) ||
        (iVar1 = FUN_8611_15ff(1,*(int *)(iVar2 * 0x41 + 0x157d) + 1,
                               *(int *)(iVar2 * 0x41 + 0x157f) + 1), iVar1 != 0)) &&
       ((iVar1 = map_distance(x,y,*(int *)(iVar2 * 0x41 + 0x157d),*(int *)(iVar2 * 0x41 + 0x157f)),
        iVar1 < local_6 && ((side == 0xe || (*(char *)(iVar2 * 0x41 + 0x1592) == side)))))) {
      local_6 = iVar1;
      local_4 = iVar2;
    }
  }
  return local_4;
}


