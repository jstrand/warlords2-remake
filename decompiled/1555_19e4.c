
byte __cdecl16far
path_step_cost(undefined4 param_1,int param_2,int param_3,int param_4,int *param_5)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  int iVar4;
  undefined2 uVar5;
  
  bVar1 = *(byte *)((int)path_cost_grid + param_2 * 0x9c + param_3);
  bVar3 = bVar1 & 7;
  if (*param_5 == 0) {
    uVar5 = (undefined2)((ulong)param_1 >> 0x10);
    iVar4 = (int)param_1;
    if (*(int *)(iVar4 + 10) == 2) {
      if (((bVar1 & 8) == 0) || ((bVar1 & 0x10) != 0)) {
        bVar2 = bVar3;
        if ((bVar1 & 7) == 0) {
          bVar2 = 2;
        }
        if (bVar2 < 3) {
          if ((bVar1 & 7) == 0) {
            bVar3 = 2;
          }
        }
        else {
          bVar3 = 2;
        }
      }
      else {
        bVar3 = 2;
      }
    }
    else if (*(int *)(iVar4 + 10) == 1) {
      if (((bVar1 & 0x10) == 0) &&
         (((param_4 != 0 && ((bVar1 & 8) == 0)) || ((param_4 == 0 && ((bVar1 & 8) != 0)))))) {
        *param_5 = 1;
      }
      if (((*(int *)(iVar4 + 8) != 0) && (2 < bVar3)) &&
         ((((*(uint *)(iVar4 + 8) & 1) != 0 && ((bVar1 & 0x20) != 0)) ||
          (((*(uint *)(iVar4 + 8) & 2) != 0 && ((bVar1 & 0x40) != 0)))))) {
        bVar3 = 2;
      }
    }
    return bVar3;
  }
  return 0x80;
}


