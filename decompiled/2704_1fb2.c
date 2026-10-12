
undefined4 __cdecl16near FUN_2704_1fb2(void)

{
  byte bVar1;
  undefined2 in_AX;
  byte bVar2;
  byte bVar3;
  uint in_DX;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  
  bVar3 = (byte)(in_DX / 100);
  bVar2 = (byte)(in_DX % 100);
  bVar1 = bVar3 % 10;
  bVar3 = bVar3 / 10 << 4 | bVar1;
  *unaff_DI = CONCAT11(bVar3,bVar2 % 10 | bVar2 / 10 << 4);
  return CONCAT22(CONCAT11(bVar3,bVar1),in_AX);
}


