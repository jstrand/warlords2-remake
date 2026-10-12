
int __cdecl16far ai_city_for_type(void)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  bool bVar6;
  int local_a;
  int local_8;
  int local_6;
  undefined1 auStack_4 [2];
  
  local_8 = -1;
  local_a = -1;
  iVar3 = city_count;
  do {
    do {
      iVar4 = iVar3 + -1;
      if (iVar3 == 0) {
        return local_8;
      }
      iVar3 = iVar4;
    } while (*(char *)(iVar4 * 0x41 + 0x1592) != (char)current_player);
    iVar2 = 4;
    do {
      bVar6 = iVar2 != 0;
      iVar2 = iVar2 + -1;
    } while (bVar6);
    bVar6 = false;
    local_6 = 0;
    uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    uVar1 = *(byte *)((int)DAT_451b_018e + iVar4 + 0x56) - 2;
    if (uVar1 < 0xd) {
                    /* WARNING: Switch is manually overridden */
      switch((undefined4)*(undefined2 *)(uVar1 * 2 + 0xdaf)) {
      case 0x5e895:
        iVar2 = FUN_623c_1a4d(iVar4,auStack_4);
        if (iVar2 == 0) {
          *(undefined1 *)((int)DAT_451b_018e + iVar4 + 0x56) = 0xe;
LAB_5db9_0d22:
          bVar6 = true;
        }
        break;
      case 0x5e8b7:
        iVar2 = FUN_623c_1a4d(iVar4,auStack_4);
        if (iVar2 == 0) goto LAB_5db9_0d22;
        *(undefined1 *)((int)DAT_451b_018e + iVar4 + 0x56) = 8;
        break;
      case 0x5e8d8:
        if (3 < *(byte *)((int)DAT_451b_018e + iVar4 + 0xba)) {
          local_6 = 4;
          goto override_jmp_5db9_0d00_case_2;
        }
        break;
      case 0x5e8eb:
override_jmp_5db9_0d00_case_2:
        iVar2 = FUN_623c_1a4d(iVar4,auStack_4);
        if (iVar2 == 0) goto LAB_5db9_0d22;
      }
    }
    if (bVar6) {
      uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      if (local_a < (int)((uint)*(byte *)((int)DAT_451b_018e + iVar4 + 0xba) - local_6)) {
        local_a = (uint)*(byte *)((int)DAT_451b_018e + iVar4 + 0xba) - local_6;
        local_8 = iVar4;
      }
    }
  } while( true );
}


