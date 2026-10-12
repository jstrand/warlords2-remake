
void __cdecl16far
FUN_1555_1ab9(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
             uint param_5)

{
  undefined2 local_da;
  undefined2 local_d8;
  undefined2 local_d6;
  undefined2 local_d4;
  uint local_d2;
  undefined2 local_d0;
  undefined1 *local_ce;
  undefined1 local_ca [200];
  
  DAT_451b_0010 = param_1;
  local_d6 = param_2;
  local_da = param_2;
  local_d4 = param_3;
  local_d8 = param_3;
  local_ce = local_ca;
  local_d0 = param_4;
  local_d2 = param_5 | 0x10;
  pathfind(&local_da);
  return;
}


