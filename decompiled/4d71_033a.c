
void __cdecl16far FUN_4d71_033a(void)

{
  int iVar1;
  int x;
  int y;
  undefined2 uVar2;
  
  for (x = 0; x < 0x70; x = x + 1) {
    for (y = 0; y < 0x9c; y = y + 1) {
      uVar2 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      if ((*(char *)((int)DAT_451b_002a + y * 0x70 + x + 0x6120) != '\x06') &&
         (*(char *)((int)DAT_451b_002a + y * 0x70 + x + 0x6120) != '\x05')) {
        iVar1 = terrain_near(x,y,6);
        if (iVar1 != 0) {
          *(undefined1 *)((int)DAT_451b_002a + y * 0x70 + x + 0x6120) = 5;
          FUN_4bed_03aa(x,y);
        }
      }
    }
  }
  return;
}


