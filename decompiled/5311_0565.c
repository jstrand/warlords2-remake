
char * __cdecl16far FUN_5311_0565(undefined4 param_1,int param_2,int param_3)

{
  char *pcVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  char *pcVar5;
  undefined2 uVar6;
  
  iVar3 = 0x32;
  pcVar5 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  while (iVar4 = iVar3 + -1, iVar3 != 0) {
    uVar6 = (undefined2)((ulong)param_1 >> 0x10);
    iVar3 = iVar4;
    if (*(int *)((int)param_1 + iVar4 * 2) != -1) {
      piVar2 = (int *)((int)param_1 + iVar4 * 2);
      pcVar1 = (char *)map_distance(param_2,param_3,*piVar2,piVar2[0x32]);
      if ((int)pcVar1 < (int)pcVar5) {
        pcVar5 = pcVar1;
      }
    }
  }
  return pcVar5;
}


