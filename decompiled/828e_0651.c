
int __cdecl16far FUN_828e_0651(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_4 = FUN_828e_04fa(param_1,param_2,current_player,0);
  iVar1 = FUN_7563_0b47(current_player,&local_6);
  local_a = local_4;
  if (iVar1 != 0) {
    iVar1 = map_distance(param_1,param_2,*(int *)(local_4 * 0x41 + 0x157d),
                         *(int *)(local_4 * 0x41 + 0x157f));
    iVar2 = map_distance(param_1,param_2,local_6,local_8);
    if (iVar2 < iVar1) {
      local_a = current_player + 100;
    }
  }
  return local_a;
}


