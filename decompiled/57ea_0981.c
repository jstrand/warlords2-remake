
char * __cdecl16far FUN_57ea_0981(int param_1,int param_2,int param_3)

{
  char *pcVar1;
  int iVar2;
  int iVar3;
  char *local_4;
  
  local_4 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  iVar2 = city_count;
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    iVar2 = iVar3;
    if ((*(char *)(*(byte *)(*(int *)(iVar3 * 0x41 + 0x157f) * 0xe0 +
                            *(int *)(iVar3 * 0x41 + 0x157d) * 2) + 0x710) == '\n') &&
       ((param_3 == -1 || (*(char *)(iVar3 * 0x41 + 0x1592) == param_3)))) {
      pcVar1 = (char *)map_distance(param_1,param_2,*(int *)(iVar3 * 0x41 + 0x157d),
                                    *(int *)(iVar3 * 0x41 + 0x157f));
      if ((int)pcVar1 < (int)local_4) {
        local_4 = pcVar1;
      }
    }
  }
  return local_4;
}


