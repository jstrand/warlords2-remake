
void __cdecl16near FUN_1d28_1314(void)

{
  char cVar1;
  int in_DX;
  int *unaff_SI;
  bool bVar2;
  
  bVar2 = CARRY1((byte)in_DX,DAT_4125_3366);
  cVar1 = ((((((((in_DX < 0) << 1 | in_DX << 1 < 0) << 1 | in_DX << 2 < 0) << 1 | in_DX << 3 < 0) <<
              1 | in_DX << 4 < 0) << 1 | in_DX << 5 < 0) << 1 | in_DX << 6 < 0) << 1 |
          in_DX << 7 < 0) + DAT_4125_3367;
  *unaff_SI = (uint)(byte)((byte)in_DX + DAT_4125_3366) << 8;
  *(char *)(unaff_SI + 1) = cVar1 + bVar2;
  return;
}


