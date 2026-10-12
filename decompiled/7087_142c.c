
void __cdecl16far
armytype_get_stats(int param_1,undefined2 *param_2,undefined2 *param_3,undefined2 *param_4,
                  undefined2 *param_5)

{
  int iVar1;
  undefined2 uVar2;
  
  armytype_lock();
  iVar1 = 0;
  do {
    if (0x1c < iVar1) {
LAB_7087_14a6:
      armytype_unlock();
      return;
    }
    uVar2 = (undefined2)((ulong)armytype_table >> 0x10);
    if (*(int *)((int)armytype_table + iVar1 * 0x3e) == param_1) {
      *param_2 = *(undefined2 *)((int)armytype_table + iVar1 * 0x3e + 0x18);
      *param_3 = *(undefined2 *)((int)armytype_table + iVar1 * 0x3e + 0x16);
      *param_4 = *(undefined2 *)((int)armytype_table + iVar1 * 0x3e + 0x1a);
      *param_5 = *(undefined2 *)((int)armytype_table + iVar1 * 0x3e + 0x1c);
      goto LAB_7087_14a6;
    }
    iVar1 = iVar1 + 1;
  } while( true );
}


