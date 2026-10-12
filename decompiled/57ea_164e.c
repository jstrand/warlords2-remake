
bool __cdecl16far FUN_57ea_164e(int param_1,int *param_2,int *param_3)

{
  int x2;
  int y2;
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int local_a;
  char *local_8;
  
  x2 = *(int *)(param_1 * 0x16 + -0x7780);
  y2 = *(int *)(param_1 * 0x16 + -0x777e);
  local_8 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  local_a = -1;
  iVar3 = city_count;
  while (iVar4 = iVar3 + -1, iVar3 != 0) {
    iVar3 = iVar4;
    if ((*(char *)(*(byte *)(*(int *)(iVar4 * 0x41 + 0x157f) * 0xe0 +
                            *(int *)(iVar4 * 0x41 + 0x157d) * 2) + 0x710) == '\n') &&
       ((*(byte *)((int)DAT_451b_018e + iVar4 + 0x11e) & 1) != 0)) {
      iVar1 = map_distance(*(int *)(iVar4 * 0x41 + 0x157d),*(int *)(iVar4 * 0x41 + 0x157f),x2,y2);
      iVar2 = dice(1,10,0);
      if (iVar1 + iVar2 < (int)local_8) {
        local_a = iVar4;
        local_8 = (char *)(iVar1 + iVar2);
      }
    }
  }
  if (local_a != -1) {
    *param_2 = *(int *)(local_a * 0x41 + 0x157d) + -1;
    *param_3 = *(int *)(local_a * 0x41 + 0x157f) + -1;
  }
  return local_a != -1;
}


