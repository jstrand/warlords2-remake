
undefined2 __cdecl16far FUN_66d4_0b36(int param_1,undefined2 *param_2,undefined2 *param_3)

{
  uint uVar1;
  undefined2 uVar2;
  
  uVar1 = (uint)*(char *)(param_1 * 0x1d + 0xcff);
  if (uVar1 < 4) {
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)(uVar1 * 2 + 0xc19)) {
    case 0x6789d:
      return 0;
    case 0x678a2:
      *param_2 = *(undefined2 *)(param_1 * 0x1d + 0xd02);
      uVar2 = *(undefined2 *)(param_1 * 0x1d + 0xd04);
      break;
    case 0x678c9:
      *param_2 = *(undefined2 *)(*(int *)(param_1 * 0x1d + 0xd00) * 0x16 + -0x7780);
      uVar2 = *(undefined2 *)(*(int *)(param_1 * 0x1d + 0xd00) * 0x16 + -0x777e);
      break;
    case 0x67910:
      *param_2 = *(undefined2 *)(*(int *)(param_1 * 0x1d + 0xd00) * 0x1f + 0x811);
      uVar2 = *(undefined2 *)(*(int *)(param_1 * 0x1d + 0xd00) * 0x1f + 0x813);
    }
    *param_3 = uVar2;
  }
  return 1;
}


