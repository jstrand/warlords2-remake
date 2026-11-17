
void __cdecl16far FUN_513d_1c1d(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined1 local_4a [22];
  int local_34;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  iVar3 = 0;
  do {
    if (7 < iVar3) {
      return;
    }
    local_4 = *(int *)(iVar3 * 0x14 + 0x189);
    local_6 = *(int *)(iVar3 * 0x14 + 0x18b);
    local_8 = city_at(local_4,local_6);
    if (-1 < local_8) {
      iVar1 = 4;
      do {
        iVar2 = iVar1 + -1;
        if (iVar1 == 0) break;
        iVar1 = iVar2;
      } while (-1 < *(char *)(local_8 * 0x41 + iVar2 + 0x1593));
      if (iVar2 < 0) {
        iVar2 = 3;
      }
      local_a = 0;
      do {
        do {
          if (0x27 < local_a) goto LAB_513d_1d01;
          local_c = dice(1,0x1c,-1);
        } while ((local_c == 5) || (((undefined1 *)&DAT_451b_0668)[local_c * 6] != '\0'));
        local_a = local_a + 1;
        iVar1 = 0;
        while ((iVar1 < 4 && (*(char *)(local_8 * 0x41 + iVar1 + 0x1593) != local_c))) {
          iVar1 = iVar1 + 1;
        }
      } while ((iVar1 < 4) || (armytype_lookup(local_c,local_4a), local_34 < 5));
LAB_513d_1d01:
      if (local_a < 0x28) {
        *(undefined1 *)(local_8 * 0x41 + iVar2 + 0x1593) = (undefined1)local_c;
      }
    }
    iVar3 = iVar3 + 1;
  } while( true );
}


