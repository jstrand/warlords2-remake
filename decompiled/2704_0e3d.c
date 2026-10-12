
void __cdecl16near FUN_2704_0e3d(void)

{
  uint in_AX;
  undefined1 in_CL;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  
  *unaff_DI = 0;
  unaff_DI[1] = 0;
  unaff_DI[2] = 0;
  unaff_DI[3] = (uint)(in_AX < 0xc001) << 0xf;
  unaff_DI[4] = in_AX;
  *(undefined1 *)(unaff_DI + 5) = in_CL;
  return;
}


