
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far site_search(void)

{
  byte *pbVar1;
  char cVar2;
  int y;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  int local_e;
  undefined2 uStack_c;
  int local_a;
  
  if (DAT_451b_1f02 != (int *)0x0) {
    uVar5 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
    if ('\0' < *(char *)((int)(int *)DAT_451b_1f02 + 7)) {
      iVar4 = *DAT_451b_1f02;
      y = ((int *)DAT_451b_1f02)[1];
      cVar2 = *(char *)(*(byte *)(y * 0xe0 + iVar4 * 2) + 0x710);
      local_e = 0;
      uStack_c = 0;
      DAT_451b_01c8 = y;
      DAT_451b_01ca = iVar4;
      if (cVar2 == '\v') {
        iVar3 = site_at(iVar4,y);
        if (iVar3 < 0) {
          cVar2 = '\a';
        }
        else {
          local_e = iVar3 * 0x1f + 0x811;
          uStack_c = 0x3c04;
          local_a = iVar3;
        }
      }
      if (_DAT_451b_1efe == 0) {
        if (cVar2 != '\v') {
          return;
        }
        if (*(char *)(local_e + 0x18) != '\x01') {
          return;
        }
      }
      else {
        if (cVar2 != '\v') {
          return;
        }
        if ((*(byte *)(y * 0xe0 + iVar4 * 2 + 1) & 0x40) != 0) {
          return;
        }
        if (*(char *)(local_e + 0x18) == '\x03') {
          hero_add_experience((int)_DAT_451b_1efe,(int)((ulong)_DAT_451b_1efe >> 0x10),3);
          FUN_6d51_1244(current_player,6,0x65,0,*(char *)((int)_DAT_451b_1efe + 10) * 0x14 + 0x223,
                        0x3c04);
          iVar3 = is_computer_turn();
          if (iVar3 == 0) {
            sage_visit(local_a);
          }
          else {
            ai_sage_visit(local_a);
          }
          pbVar1 = (byte *)(y * 0xe0 + iVar4 * 2 + 1);
          *pbVar1 = *pbVar1 | 0x40;
          return;
        }
        if (*(char *)(local_e + 0x18) != '\x01') {
          ruin_search(local_e,uStack_c,iVar4,y);
          return;
        }
        iVar4 = is_computer_turn();
        if (iVar4 == 0) {
          auto_ui_temple();
          return;
        }
      }
      temple_bless(local_a);
    }
  }
  return;
}


