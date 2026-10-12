
void __cdecl16far FUN_1997_06bb(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 uVar3;
  
  uVar1 = *(undefined4 *)(param_1 * 0xc + 0x1f57);
  uVar3 = (undefined2)((ulong)uVar1 >> 0x10);
  if (*(long *)((int)uVar1 + 0x1a) != *(long *)(*(int *)(param_1 * 0xc + 0x1f57) + 0x1e)) {
    for (iVar2 = 0; iVar2 < 4; iVar2 = iVar2 + 1) {
      uVar1 = *(undefined4 *)(param_1 * 0xc + 0x1f57);
      mem_unlock(*(undefined4 *)((int)uVar1 + iVar2 * 4 + 10));
    }
    if ((((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 0x20) != 0) {
      uVar1 = *(undefined4 *)(param_1 * 0xc + 0x1f5c);
      mem_unlock(*(undefined4 *)((int)uVar1 + 10));
    }
  }
  return;
}


