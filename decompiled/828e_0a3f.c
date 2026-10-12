
void __cdecl16far FUN_828e_0a3f(int param_1,int param_2,int param_3,int param_4)

{
  if (((opt_hidden_map == 0) || ((*(byte *)(param_2 * 0x70 + param_1) & 0x20) != 0)) &&
     ((opt_hidden_map == 0 || ((*(byte *)(param_4 * 0x70 + param_3) & 0x20) != 0)))) {
    FUN_24d0_0002();
    FUN_2133_00b8(0x6795,0x4125,param_1 * 2 + -2,param_2 * 2 + -2,param_3 * 2 + -2,param_4 * 2 + -2,
                  0);
  }
  return;
}


