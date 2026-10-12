
void __cdecl16far
FUN_59bf_0cd7(int param_1,undefined2 param_2,undefined4 param_3,undefined4 param_4,int param_5,
             int param_6)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  
  FUN_5c52_0000(param_1,param_2,param_3,0xf,0x2d0000);
  for (iVar3 = 0; iVar3 < param_5; iVar3 = iVar3 + 1) {
    if (param_5 == 8) {
      uVar1 = *(undefined2 *)(iVar3 * 2 + 0x7c8);
    }
    else {
      uVar1 = 0xf;
    }
    iVar2 = *(int *)((int)param_4 + iVar3 * 2) + param_6;
    if ((iVar2 == 0) || (iVar2 == -1)) {
      FUN_5c52_005a(param_1 + iVar3 * 4 + 10,param_2,0x7f2,0x4125,uVar1,0);
    }
    else {
      if (1000 < iVar2) {
        iVar2 = 999;
      }
      FUN_5c52_0099(param_1 + iVar3 * 4 + 10,param_2,iVar2,3);
    }
  }
  return;
}


