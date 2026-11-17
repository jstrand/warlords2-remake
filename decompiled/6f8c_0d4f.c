
int __cdecl16far army_can_arrive(int type,int x,int y)

{
  int iVar1;
  
  iVar1 = city_at(x,y);
  if ((iVar1 < 0) || (*(char *)(iVar1 * 0x41 + 0x1592) == current_player)) {
    if (((undefined1 *)&army_move_flags)[type * 6] == '\0') {
      if (*(int *)((undefined1 *)&terrain_move_cost +
                  *(char *)(*(byte *)(y * 0xe0 + x * 2) + 0x710) * 2) < 1) {
        iVar1 = 0;
      }
      else {
        iVar1 = 1;
      }
    }
    else {
      iVar1 = 1;
    }
  }
  else {
    iVar1 = 0;
  }
  return iVar1;
}


