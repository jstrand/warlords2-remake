
void __cdecl16far FUN_1c8c_041f(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  undefined2 uVar4;
  int local_4;
  
  local_4 = *DAT_451b_1f02;
  iVar2 = ((int *)DAT_451b_1f02)[1];
  FUN_1a8b_0b4d(&local_4);
  iVar1 = auto_tutorial_hook_2(local_4,iVar2);
  if ((iVar1 == 3) || (iVar1 == 5)) {
    attack_tile(local_4,iVar2);
  }
  if (DAT_4125_2bda != 0) {
    iVar2 = 8;
    while (iVar1 = iVar2 + -1, iVar2 != 0) {
      iVar2 = iVar1;
      if (((undefined2 *)&DAT_451b_1ede)[iVar1 * 2] != 0 ||
          *(int *)((undefined *)&DAT_451b_1ee0 + iVar1 * 4) != 0) {
        uVar4 = (undefined2)
                ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar1 * 2) >> 0x10);
        *(undefined2 *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar1 * 2) + 0x12) =
             0xffff;
        *(undefined2 *)(((undefined2 *)&DAT_451b_1ede)[iVar1 * 2] + 0x14) = 0xffff;
      }
    }
  }
  uVar4 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
  piVar3 = (int *)DAT_451b_1f02;
  piVar3[9] = -1;
  piVar3[10] = -1;
  return;
}


