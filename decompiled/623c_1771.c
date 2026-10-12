
int __cdecl16far FUN_623c_1771(int param_1,undefined2 param_2,undefined2 param_3)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar3 = FUN_623c_15c5(param_2,param_3);
  uVar1 = *DAT_451b_1f02;
  uVar2 = ((undefined2 *)DAT_451b_1f02)[1];
  iVar4 = (int)*(char *)(param_1 * 0x41 + 0x1592);
  if ((iVar4 == 0xf) || ((*(byte *)(iVar4 * 8 + current_player + 0x153b) & 3) == 2)) {
    iVar5 = 1;
  }
  else {
    iVar5 = 0;
  }
  if (((iVar4 != 0xf) && (*(int *)(iVar4 * 2 + 0xd0) == 0)) && (iVar4 = dice(1,4,-1), iVar4 == 0)) {
    iVar5 = 1;
  }
  if ((iVar5 == 0) || (iVar4 = param_1, iVar3 < 0x33)) {
    FUN_1555_1ab9(0xf,uVar1,uVar2,DAT_451b_1e9c,DAT_451b_1e9a);
    iVar4 = FUN_623c_1885(param_2,param_3,iVar5);
    if (iVar4 == -1) {
      iVar4 = param_1;
    }
  }
  return iVar4;
}


