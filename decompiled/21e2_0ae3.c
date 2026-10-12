
int __cdecl16far FUN_21e2_0ae3(int param_1)

{
  undefined4 uVar1;
  undefined2 uVar2;
  
  uVar1 = *(undefined4 *)((int)DAT_451b_30e9 + 0x61);
  if (((*(uint *)((int)uVar1 + 0x26) & 2) != 0) && ((*(byte *)(param_1 + 0x3a5f) & 8) != 0)) {
    param_1 = toupper(param_1);
  }
  uVar2 = (undefined2)((ulong)DAT_451b_30e9 >> 0x10);
  uVar1 = *(undefined4 *)((int)DAT_451b_30e9 + 0x61);
  param_1 = param_1 - *(int *)((int)uVar1 + 0x10);
  if (param_1 < 0) {
    param_1 = 0;
  }
  uVar1 = *(undefined4 *)((int)DAT_451b_30e9 + 0x61);
  if (*(int *)((int)uVar1 + 0x12) < param_1) {
    param_1 = 0;
  }
  return param_1;
}


