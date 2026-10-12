
undefined2 __cdecl16far FUN_5311_0e22(undefined4 param_1,int *param_2,int *param_3)

{
  int y1;
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  int local_6;
  int local_4;
  
  do {
    iVar4 = 0x50;
    do {
      iVar3 = (int)param_1;
      uVar7 = (undefined2)((ulong)param_1 >> 0x10);
      iVar1 = -1;
      if (iVar4 == 0) break;
      iVar1 = dice(1,city_count,-1);
      iVar4 = iVar4 + -1;
    } while (*(char *)(iVar3 + iVar1 + 600) != '\0');
    if (iVar1 == -1) {
      return 0;
    }
    *(undefined1 *)(iVar3 + iVar1 + 600) = 1;
    iVar4 = *(int *)(iVar1 * 0x41 + 0x157d);
    y1 = *(int *)(iVar1 * 0x41 + 0x157f);
    local_6 = -1;
    local_4 = -1;
    iVar5 = city_count;
    while (iVar6 = iVar5 + -1, iVar5 != 0) {
      iVar5 = iVar6;
      if ((((*(char *)(iVar3 + iVar6 + 600) == '\0') &&
           (iVar2 = map_distance(iVar4,y1,*(int *)(iVar6 * 0x41 + 0x157d),
                                 *(int *)(iVar6 * 0x41 + 0x157f)), 0x1d < iVar2)) && (iVar2 < 0x47))
         && (iVar2 = dice(1,1000,0), local_4 < iVar2)) {
        local_6 = iVar6;
        local_4 = iVar2;
      }
    }
    if (local_6 != -1) {
      *(undefined1 *)(iVar3 + local_6 + 600) = 1;
      *param_2 = iVar1;
      *param_3 = local_6;
      return 1;
    }
  } while( true );
}


