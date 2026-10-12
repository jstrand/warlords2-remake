
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_6536_186e(void)

{
  int *piVar1;
  int iVar2;
  undefined2 in_DX;
  int iVar3;
  undefined2 unaff_SS;
  int local_4;
  
  iVar2 = army_count;
  do {
    iVar3 = iVar2 + -1;
    if (iVar2 == 0) break;
    in_DX = 0x2e1d;
    iVar2 = iVar3;
  } while ((_DAT_451b_1f00 != 0x2e1d) || ((int *)(iVar3 * 0x16 + -0x7780) != DAT_451b_1efe));
  list_carried_items(3,*_DAT_451b_1efe,DAT_451b_1efe[1],0);
  for (iVar2 = 0; iVar2 < DAT_451b_08ce; iVar2 = iVar2 + 1) {
    *(int *)(((undefined2 *)&DAT_451b_0c9c)[iVar2] * 0x1d + 0xd00) = iVar3;
    *(undefined1 *)(((undefined2 *)&DAT_451b_0c9c)[iVar2] * 0x1d + 0xcff) = 3;
  }
  FUN_54f6_069c();
  piVar1 = (int *)armies_at_tile(*_DAT_451b_1efe,((int *)_DAT_451b_1efe)[1],
                                 (int *)CONCAT22(unaff_SS,&local_4));
  DAT_451b_1f02 = (int *)CONCAT22(in_DX,piVar1);
  if (_DAT_451b_1efe != (int *)0x0) {
    DAT_451b_1f02 = _DAT_451b_1efe;
  }
  FUN_1b62_09ec(local_4,0);
  FUN_8c07_06eb(DAT_451b_1f02);
  FUN_8065_011f();
  quest_check(3,0,0);
  return;
}


