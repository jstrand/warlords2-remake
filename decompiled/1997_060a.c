
void __cdecl16far FUN_1997_060a(int param_1)

{
  undefined4 uVar1;
  undefined2 in_DX;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  int local_4;
  
  uVar1 = *(undefined4 *)(param_1 * 0xc + 0x1f57);
  uVar3 = (undefined2)((ulong)uVar1 >> 0x10);
  if (*(long *)((int)uVar1 + 0x1a) != *(long *)(*(int *)(param_1 * 0xc + 0x1f57) + 0x1e)) {
    for (local_4 = 0; local_4 < 4; local_4 = local_4 + 1) {
      uVar1 = *(undefined4 *)(param_1 * 0xc + 0x1f57);
      uVar3 = mem_lock(*(undefined4 *)((int)uVar1 + local_4 * 4 + 10));
      uVar1 = *(undefined4 *)(param_1 * 0xc + 0x1f57);
      uVar4 = (undefined2)((ulong)uVar1 >> 0x10);
      iVar2 = (int)uVar1 + local_4 * 4;
      *(undefined2 *)(iVar2 + 0x1c) = in_DX;
      *(undefined2 *)(iVar2 + 0x1a) = uVar3;
    }
    if ((((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 0x20) != 0) {
      uVar1 = *(undefined4 *)(param_1 * 0xc + 0x1f5c);
      uVar3 = mem_lock(*(undefined4 *)((int)uVar1 + 10));
      uVar1 = *(undefined4 *)(param_1 * 0xc + 0x1f5c);
      uVar4 = (undefined2)((ulong)uVar1 >> 0x10);
      iVar2 = (int)uVar1;
      *(undefined2 *)(iVar2 + 0x1c) = in_DX;
      *(undefined2 *)(iVar2 + 0x1a) = uVar3;
    }
  }
  return;
}


