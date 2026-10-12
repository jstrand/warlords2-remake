
void __cdecl16far FUN_828e_08cd(int param_1,int param_2,undefined2 param_3)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined1 local_c [8];
  undefined2 local_4;
  
  local_4 = 0;
  if ((opt_hidden_map == 0) || ((*(byte *)(param_2 * 0x70 + param_1) & 0x20) != 0)) {
    iVar2 = param_1 * 2;
    iVar3 = param_2 * 2 + -1;
    uVar1 = FUN_216d_002e(local_c);
    FUN_24d0_0497(0x6795,0x4125,uVar1,param_1,param_3);
    FUN_2012_0bb7(0x6795,0x4125,iVar2 + -1,iVar3,6,local_4);
    FUN_2012_0bb7(0x6795,0x4125,iVar2,param_2 * 2 + 4,4,local_4);
    FUN_2012_0f08(0x6795,0x4125,iVar2 + -1,iVar3,5,local_4);
    FUN_2012_0f08(0x6795,0x4125,iVar2 + 4,iVar3,5,local_4);
  }
  return;
}


