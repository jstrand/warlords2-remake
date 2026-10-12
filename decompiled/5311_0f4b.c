
void __cdecl16far FUN_5311_0f4b(undefined4 param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = city_count;
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    iVar1 = map_distance(param_2,param_3,*(int *)(iVar3 * 0x41 + 0x157d),
                         *(int *)(iVar3 * 0x41 + 0x157f));
    iVar2 = iVar3;
    if (iVar1 < 10) {
      *(undefined1 *)((int)param_1 + iVar3 + 600) = 1;
    }
  }
  return;
}


