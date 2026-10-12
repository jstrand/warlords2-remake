
void __cdecl16far FUN_4c49_0ce7(void)

{
  int iVar1;
  int x;
  int y;
  
  for (x = 0; x < 0x70; x = x + 1) {
    for (y = 0; y < 0x9c; y = y + 1) {
      if (*(char *)((int)DAT_451b_002a + y * 0x70 + x + 0x6120) == '\x02') {
        iVar1 = FUN_4c49_117d(x,y);
        if (iVar1 != 0) {
          *(undefined1 *)((int)DAT_451b_002a + y * 0x70 + x + 0x6120) = 3;
          FUN_4bed_03aa(x,y);
        }
        iVar1 = terrain_near(x,y,5);
        if (iVar1 != 0) {
          *(undefined1 *)((int)DAT_451b_002a + y * 0x70 + x + 0x6120) = 3;
          FUN_4bed_03aa(x,y);
        }
        iVar1 = terrain_near(x,y,6);
        if (iVar1 != 0) {
          *(undefined1 *)((int)DAT_451b_002a + y * 0x70 + x + 0x6120) = 3;
          FUN_4bed_03aa(x,y);
        }
      }
    }
  }
  return;
}


