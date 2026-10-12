
void __cdecl16far armytype_lookup(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined2 uVar2;
  
  armytype_lock();
  iVar1 = 0;
  do {
    if (0x1c < iVar1) {
LAB_7087_14ee:
      armytype_unlock();
      return;
    }
    uVar2 = (undefined2)((ulong)armytype_table >> 0x10);
    if (*(int *)((int)armytype_table + iVar1 * 0x3e) == param_1) {
      __F_SCOPY((int)armytype_table + iVar1 * 0x3e,uVar2,param_2);
      goto LAB_7087_14ee;
    }
    iVar1 = iVar1 + 1;
  } while( true );
}


