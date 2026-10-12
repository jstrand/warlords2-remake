
undefined2 __cdecl16far FUN_6f8c_0d4f(int param_1,int param_2,int param_3)

{
  int iVar1;
  undefined2 uVar2;
  
  iVar1 = FUN_828e_044a(param_2,param_3);
  if ((iVar1 < 0) || (*(char *)(iVar1 * 0x41 + 0x1592) == current_player)) {
    if (((undefined1 *)&army_move_flags)[param_1 * 6] == '\0') {
      if (*(int *)((undefined1 *)&terrain_move_cost +
                  *(char *)(*(byte *)(param_3 * 0xe0 + param_2 * 2) + 0x710) * 2) < 1) {
        uVar2 = 0;
      }
      else {
        uVar2 = 1;
      }
    }
    else {
      uVar2 = 1;
    }
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}


