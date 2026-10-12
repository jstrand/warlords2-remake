
void __cdecl16far FUN_7087_11f4(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined4 local_6;
  
  armytype_lock();
  iVar1 = 0;
  for (iVar2 = 0; iVar3 = param_2, iVar2 < 0x1c; iVar2 = iVar2 + 1) {
    if (-1 < *(int *)((int)armytype_table + iVar2 * 0x3e + 0x1e)) {
      iVar3 = iVar2;
      if (iVar1 == param_2) break;
      iVar1 = iVar1 + 1;
    }
  }
  if (param_1 != 0) {
    FUN_22bf_0af4(&local_6);
    uVar4 = (undefined2)((ulong)armytype_table >> 0x10);
    iVar1 = (int)armytype_table;
    ui_army_type_info(*(undefined2 *)(iVar1 + iVar3 * 0x3e),
                      *(undefined2 *)(iVar1 + iVar3 * 0x3e + 0x18),
                      *(undefined2 *)(iVar1 + iVar3 * 0x3e + 0x16),
                      *(undefined2 *)(iVar1 + iVar3 * 0x3e + 0x1a),
                      *(undefined2 *)(iVar1 + iVar3 * 0x3e + 0x1c),local_6);
  }
  armytype_unlock();
  return;
}


