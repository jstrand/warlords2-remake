
void __cdecl16far draw_map_figure(int x,int y,int figure)

{
  int iVar1;
  
  y = y * 2 + -6;
  iVar1 = (x * 2 + -2 >> 3) << 3;
  if (iVar1 < 0) {
    iVar1 = 0;
  }
  if (y < 0) {
    y = 0;
  }
  FUN_1997_034a(9);
  (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,*(undefined2 *)(figure * 8 + 0x2cae),
                   *(undefined2 *)(figure * 8 + 0x2cb0),*(undefined2 *)(figure * 8 + 0x2cb2),
                   *(undefined2 *)(figure * 8 + 0x2cb4),0x6795,0x4125,iVar1,y,0x576c,0x4125,
                   *(undefined2 *)(figure * 8 + 0x2cae),*(undefined2 *)(figure * 8 + 0x2cb0));
  return;
}


