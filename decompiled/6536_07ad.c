
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

bool __cdecl16far guardian_fight_hero_dies(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  list_carried_items(2,0,_DAT_451b_1efe);
  iVar3 = 0;
  for (iVar1 = 0; iVar1 < DAT_451b_08ce; iVar1 = iVar1 + 1) {
    if (*(char *)(((undefined2 *)&DAT_451b_0c9c)[iVar1] * 0x1d + 0xcfd) == '\x01') {
      iVar3 = iVar3 + *(char *)(((undefined2 *)&DAT_451b_0c9c)[iVar1] * 0x1d + 0xcfe);
    }
  }
  iVar4 = 0;
  iVar1 = army_count;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if ((*(int *)(iVar2 * 0x16 + -0x7780) == *_DAT_451b_1efe) &&
       (*(int *)(iVar2 * 0x16 + -0x777e) == ((int *)_DAT_451b_1efe)[1])) {
      iVar4 = iVar4 + 1;
    }
  }
  iVar1 = dice(1,100,0);
  return iVar4 * 3 +
         (((char)((int *)_DAT_451b_1efe)[4] + iVar3) -
         *(int *)(*(char *)((int)param_1 + 0x1a) * 2 + 0x1007)) * 5 + 0x5a < iVar1;
}


