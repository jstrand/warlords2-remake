
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_177b_0161(uint param_1)

{
  if (((param_1 & 3) == 0) && (_LAB_4125_124b_1 != 0)) {
    FUN_177b_01a1(_LAB_4125_1251_1,_LAB_4125_1253_1,
                  *(int *)(_LAB_4125_124b_1 * 2 + 0x125c) + _LAB_4125_124d_1,_LAB_4125_124f_1);
    _LAB_4125_124d_1 = _LAB_4125_124d_1 + 1 & 3;
  }
  return;
}


