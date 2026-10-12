
void __cdecl16far FUN_623c_0c7e(int *param_1,int param_2,int param_3)

{
  int *piVar1;
  undefined2 uVar2;
  int local_6;
  int local_4;
  
  local_4 = -1;
  local_6 = -1;
  if (param_2 != 4) {
    piVar1 = (int *)param_1;
    uVar2 = (undefined2)((ulong)param_1 >> 0x10);
    if (param_2 == 1) {
      local_4 = *(int *)(param_3 * 0x41 + 0x157d);
      local_6 = *(int *)(param_3 * 0x41 + 0x157f);
      if (*(char *)(param_3 * 0x41 + 0x1592) == current_player) {
        local_4 = local_4 + 1;
        local_6 = local_6 + 1;
      }
      else {
        FUN_623c_0d88(*param_1,piVar1[1],&local_4);
      }
    }
    else if (param_2 == 2) {
      local_4 = *(int *)(param_3 * 0x1d + 0xd02);
      local_6 = *(int *)(param_3 * 0x1d + 0xd04);
    }
    else if (param_2 == 3) {
      local_4 = *(int *)(param_3 * 0x1f + 0x811);
      local_6 = *(int *)(param_3 * 0x1f + 0x813);
    }
    if (local_4 == -1) {
      local_4 = *param_1;
    }
    piVar1[9] = local_4;
    if (local_6 == -1) {
      local_6 = piVar1[1];
    }
    piVar1[10] = local_6;
  }
  return;
}


