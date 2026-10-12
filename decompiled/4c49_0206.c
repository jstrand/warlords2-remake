
void __cdecl16far FUN_4c49_0206(void)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  
  for (iVar2 = 0; iVar2 < 4; iVar2 = iVar2 + 1) {
    iVar3 = (iVar2 + 1) % 4;
    uVar7 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar4 = (int)DAT_451b_002a;
    iVar5 = iVar4 + iVar2 * 4;
    iVar6 = iVar4 + iVar3 * 4;
    if ((float)(0x38 - *(int *)(iVar5 + 0x254)) / (float)(0x4e - *(int *)(iVar5 + 0x256)) <
        (float)(0x38 - *(int *)(iVar6 + 0x244)) / (float)(0x4e - *(int *)(iVar6 + 0x246))) {
      uVar1 = *(undefined4 *)(iVar4 + iVar2 * 4 + 0x254);
      *(undefined4 *)(iVar4 + iVar2 * 4 + 0x254) = *(undefined4 *)(iVar4 + iVar3 * 4 + 0x244);
      *(undefined4 *)((int)DAT_451b_002a + iVar3 * 4 + 0x244) = uVar1;
    }
  }
  return;
}


