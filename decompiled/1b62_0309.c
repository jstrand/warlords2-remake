
int __cdecl16far FUN_1b62_0309(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined4 local_6;
  
  local_6 = 0;
  iVar1 = 0;
  do {
    if (999 < iVar1) {
LAB_1b62_034c:
      if (local_6 == 0) {
        iVar1 = 0;
      }
      else {
        if (army_count <= iVar1) {
          army_count = iVar1 + 1;
        }
        uVar2 = (undefined2)((ulong)local_6 >> 0x10);
        iVar1 = (int)local_6;
        *(undefined1 *)(iVar1 + 5) = (undefined1)current_player;
        *(undefined2 *)(iVar1 + 0xc) = 0;
        *(uint *)(iVar1 + 0xc) = *(uint *)(iVar1 + 0xc) | 1;
        *(byte *)(iVar1 + 0xf) = *(byte *)(iVar1 + 0xf) & 1;
        *(byte *)(iVar1 + 0xe) = *(byte *)(iVar1 + 0xe) & 0xf0;
        *(undefined1 *)(iVar1 + 0xe) = *(undefined1 *)(iVar1 + 0xe);
        *(undefined1 *)(iVar1 + 0x10) = 0xff;
        *(undefined1 *)(iVar1 + 0x11) = 0;
        *(undefined2 *)(iVar1 + 0x12) = 0xffff;
        *(undefined2 *)(iVar1 + 0x14) = 0xffff;
      }
      return iVar1;
    }
    if (*(char *)(iVar1 * 0x16 + -0x777b) == -1) {
      local_6 = CONCAT22(0x2e1d,iVar1 * 0x16 + -0x7780);
      goto LAB_1b62_034c;
    }
    iVar1 = iVar1 + 1;
  } while( true );
}


