
int __cdecl16far dice(int n,int sides,int bonus)

{
  int iVar1;
  undefined2 local_6;
  undefined2 local_4;
  
  local_6 = 0;
  if (dice_seeded == 0) {
    dice_seeded = 1;
    dice_seed();
  }
  iVar1 = bonus;
  if (sides != 0) {
    for (local_4 = 0; local_4 < n; local_4 = local_4 + 1) {
      rand();
      iVar1 = __ftol();
      local_6 = local_6 + iVar1;
    }
    iVar1 = local_6 + bonus;
    if (iVar1 < n + bonus) {
      iVar1 = n + bonus;
    }
    else if (n * sides + bonus < iVar1) {
      iVar1 = n * sides + bonus;
    }
  }
  return iVar1;
}


