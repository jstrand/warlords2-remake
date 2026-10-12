
int __cdecl16far ai_hero_city(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 local_4;
  
  local_4 = 0;
  iVar3 = city_count;
  do {
    do {
      iVar4 = iVar3 + -1;
      if (iVar3 == 0) {
        return param_1;
      }
      iVar3 = iVar4;
    } while (*(char *)(iVar4 * 0x41 + 0x1592) != current_player);
    iVar1 = 0;
    uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar2 = (int)DAT_451b_018e;
    if (*(char *)(iVar2 + iVar4 + 0x56) == '\a') {
      iVar1 = 100;
LAB_5db9_0984:
      iVar1 = dice(1,100,iVar1);
    }
    else {
      if (*(char *)(iVar2 + iVar4 + 0x56) == '\x02') {
        iVar1 = 0x32;
        goto LAB_5db9_0984;
      }
      if (*(char *)(iVar2 + iVar4 + 0x56) == '\x03') {
        iVar1 = 0;
        goto LAB_5db9_0984;
      }
    }
    if (local_4 < iVar1) {
      param_1 = iVar4;
      local_4 = iVar1;
    }
  } while( true );
}


