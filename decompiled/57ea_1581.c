
void __cdecl16far FUN_57ea_1581(int param_1,undefined2 param_2,undefined2 param_3)

{
  uint *puVar1;
  byte *pbVar2;
  int iVar3;
  undefined2 uVar4;
  
  *(undefined2 *)(param_1 * 0x16 + -0x776e) = param_2;
  *(undefined2 *)(param_1 * 0x16 + -0x776c) = param_3;
  FUN_8c07_06eb(param_1 * 0x16 + -0x7780,0x2e1d);
  uVar4 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
  iVar3 = move_stack_to(0x8c07,*(undefined2 *)((int)DAT_451b_1f02 + 0x12),
                        *(undefined2 *)((int)DAT_451b_1f02 + 0x14),0);
  if (iVar3 == 1) {
    puVar1 = (uint *)(param_1 * 0x16 + -0x7774);
    *puVar1 = *puVar1 & 0xffdf;
    pbVar2 = (byte *)(param_1 * 0x16 + -0x7772);
    *pbVar2 = *pbVar2 & 0xf0;
    pbVar2 = (byte *)(param_1 * 0x16 + -0x7771);
    *pbVar2 = *pbVar2 & 1;
  }
  puVar1 = (uint *)(param_1 * 0x16 + -0x7774);
  *puVar1 = *puVar1 | 0x40;
  *(undefined2 *)(param_1 * 0x16 + -0x776c) = 0xffff;
  *(undefined2 *)(param_1 * 0x16 + -0x776e) = 0xffff;
  *(undefined1 *)(param_1 * 0x16 + -0x776f) = 0;
  return;
}


