
void __cdecl16far FUN_828e_099e(int param_1,int param_2,undefined2 param_3)

{
  int iVar1;
  int iVar2;
  
  if ((opt_hidden_map == 0) || ((*(byte *)(param_2 * 0x70 + param_1) & 0x20) != 0)) {
    iVar2 = param_1 * 2 + -2;
    iVar1 = param_2 * 2 + -2;
    FUN_2012_0bb7(0x6795,0x4125,iVar2,iVar1,8,param_3);
    FUN_2012_0bb7(0x6795,0x4125,iVar2,param_2 * 2 + 5,8,param_3);
    FUN_2012_0f08(0x6795,0x4125,iVar2,iVar1,8,param_3);
    FUN_2012_0f08(0x6795,0x4125,param_1 * 2 + 5,iVar1,8,param_3);
  }
  return;
}


