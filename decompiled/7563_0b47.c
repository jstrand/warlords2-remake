
undefined2 __cdecl16far FUN_7563_0b47(int param_1,undefined2 *param_2,undefined2 *param_3)

{
  undefined2 uVar1;
  
  *param_2 = 0xffff;
  *param_3 = 0xffff;
  if ((*(char *)(param_1 * 0x1d + 0xcff) == '\x01') &&
     ((*(byte *)(*(int *)(param_1 * 0x1d + 0xd04) * 0x70 + *(int *)(param_1 * 0x1d + 0xd02)) & 0x40)
      != 0)) {
    *param_2 = *(undefined2 *)(param_1 * 0x1d + 0xd02);
    *param_3 = *(undefined2 *)(param_1 * 0x1d + 0xd04);
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}


