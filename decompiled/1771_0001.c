
void __cdecl16far FUN_1771_0001(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined4 local_e;
  undefined2 local_a;
  undefined2 uStack_8;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  if ((*(long *)(iVar1 + 0xc) == 0) && (0 < *(int *)(iVar1 + 0x1f))) {
    FUN_216d_002e(&local_a);
    FUN_216d_000e(&local_e);
    FUN_22bf_07a5();
    __F_SPUSH(local_e);
    FUN_1997_027e(*(undefined2 *)(iVar1 + 0x1f));
    uStack_8 = 0x1997;
    local_a = 0x77ad;
    FUN_22bf_0757();
    *(undefined1 *)(iVar1 + 0x12) = *(undefined1 *)(iVar1 + 4);
  }
  return;
}


