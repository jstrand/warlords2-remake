
void __cdecl16far turn_order(void)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  if (opt_random_turns == 0) {
    for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
      *(int *)(iVar4 * 2 + 0x163) = iVar4;
    }
  }
  else {
    for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
      *(int *)(iVar4 * 2 + 0x163) = iVar4;
    }
    for (iVar4 = 0; iVar4 < 0x14; iVar4 = iVar4 + 1) {
      iVar2 = dice(1,8,-1);
      iVar3 = dice(1,8,-1);
      uVar1 = *(undefined2 *)(iVar3 * 2 + 0x163);
      *(undefined2 *)(iVar3 * 2 + 0x163) = *(undefined2 *)(iVar2 * 2 + 0x163);
      *(undefined2 *)(iVar2 * 2 + 0x163) = uVar1;
    }
  }
  DAT_3c04_0173 = 0;
  return;
}


