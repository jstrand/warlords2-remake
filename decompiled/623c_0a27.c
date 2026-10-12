
int __cdecl16far FUN_623c_0a27(int param_1,int param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 local_6;
  undefined2 local_4;
  
  local_4 = -1;
  local_6 = 1000;
  iVar2 = city_count;
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    iVar2 = iVar3;
    if ((*(char *)(*(byte *)(*(int *)(iVar3 * 0x41 + 0x157f) * 0xe0 +
                            *(int *)(iVar3 * 0x41 + 0x157d) * 2) + 0x710) == '\n') &&
       (*(byte *)(iVar3 * 0x41 + 0x15ac) == param_3)) {
      iVar1 = map_distance(param_1,param_2,*(int *)(iVar3 * 0x41 + 0x157d),
                           *(int *)(iVar3 * 0x41 + 0x157f));
      if (iVar1 < local_6) {
        local_6 = iVar1;
        local_4 = iVar3;
      }
    }
  }
  return local_4;
}


