
undefined2 __cdecl16far FUN_7087_14f4(int param_1)

{
  int iVar1;
  undefined2 uVar2;
  
  armytype_lock();
  iVar1 = 0;
  do {
    if (0x1c < iVar1) {
LAB_7087_1539:
      armytype_unlock();
      return 0x4468;
    }
    uVar2 = (undefined2)((ulong)armytype_table >> 0x10);
    if (*(int *)((int)armytype_table + iVar1 * 0x3e) == param_1) {
      strcpy((char *)0x41254468,
             (char *)CONCAT22(uVar2,(char *)((int)armytype_table + iVar1 * 0x3e + 2)));
      goto LAB_7087_1539;
    }
    iVar1 = iVar1 + 1;
  } while( true );
}


