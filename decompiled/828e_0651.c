
int __cdecl16far nearest_vector_target(int x,int y)

{
  int iVar1;
  int iVar2;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_4 = nearest_city(x,y,current_player,0);
  iVar1 = standard_planted_at(current_player,&local_6);
  local_a = local_4;
  if (iVar1 != 0) {
    iVar1 = map_distance(x,y,*(int *)(local_4 * 0x41 + 0x157d),*(int *)(local_4 * 0x41 + 0x157f));
    iVar2 = map_distance(x,y,local_6,local_8);
    if (iVar2 < iVar1) {
      local_a = current_player + 100;
    }
  }
  return local_a;
}


