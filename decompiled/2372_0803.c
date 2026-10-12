
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far FUN_2372_0803(int param_1)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  undefined1 local_a [8];
  
  iVar1 = ((int *)DAT_451b_2fc8)[1];
  iVar4 = *DAT_451b_2fc8 + param_1 * 0x2e;
  iVar5 = iVar4;
  iVar2 = FUN_2012_01fe(0x6f2c,0x4125,*(undefined2 *)(iVar4 + 0x24),*(undefined2 *)(iVar4 + 0x26));
  if (iVar2 == 0) {
    uVar3 = 0;
  }
  else {
    if ((_DAT_451b_2fd6 != 0) && ((DAT_451b_3001 & 10) == 0)) {
      for (iVar2 = 0; iVar2 < (int)(uint)DAT_451b_3000; iVar2 = iVar2 + 1) {
        uVar3 = mem_lock(((undefined2 *)&DAT_451b_2fd6)[iVar2 * 2],
                         *(undefined2 *)((undefined *)&DAT_451b_2fd8 + iVar2 * 4));
        ((undefined2 *)&DAT_451b_2fe8)[iVar2 * 2] = iVar5;
        ((undefined2 *)&DAT_451b_2fe6)[iVar2 * 2] = uVar3;
      }
    }
    FUN_22bf_07a5();
    (*DAT_451b_2a74)(0x22bf,0x342a,0x4125,*(undefined2 *)(iVar4 + 0x16),
                     *(undefined2 *)(iVar4 + 0x18),*(undefined2 *)(iVar4 + 0x24),
                     *(undefined2 *)(iVar4 + 0x26),0x6f2c,0x4125,0,0);
    FUN_216d_002e(local_a);
    FUN_24d0_0497(0x342a,0x4125,local_a);
    FUN_2012_0bb7(0x342a,0x4125,*(undefined2 *)(iVar4 + 0x16),
                  *(int *)(iVar4 + 0x18) + *(int *)(iVar4 + 0x26) + -2,*(int *)(iVar4 + 0x24) + -2,0
                 );
    FUN_2012_0bb7(0x342a,0x4125,*(int *)(iVar4 + 0x16) + 2,
                  *(int *)(iVar4 + 0x18) + *(int *)(iVar4 + 0x26) + -1,*(int *)(iVar4 + 0x24) + -2,0
                 );
    FUN_2012_0f08(0x342a,0x4125,*(undefined2 *)(iVar4 + 0x16),*(undefined2 *)(iVar4 + 0x18),
                  *(int *)(iVar4 + 0x26) + -2,0);
    FUN_2012_0f08(0x342a,0x4125,*(int *)(iVar4 + 0x16) + *(int *)(iVar4 + 0x24) + -2,
                  *(undefined2 *)(iVar4 + 0x18),*(int *)(iVar4 + 0x26) + -1,0);
    FUN_2012_0f08(0x342a,0x4125,*(int *)(iVar4 + 0x16) + *(int *)(iVar4 + 0x24) + -1,
                  *(int *)(iVar4 + 0x18) + 1,*(int *)(iVar4 + 0x26) + -1,0);
    FUN_22bf_0757();
    uVar3 = 1;
  }
  return uVar3;
}


