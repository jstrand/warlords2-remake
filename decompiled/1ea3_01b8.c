
undefined1 __cdecl16far FUN_1ea3_01b8(void)

{
  undefined1 uVar1;
  int iVar2;
  int in_DX;
  int iVar3;
  
  out(in_DX,0xf);
  iVar3 = in_DX + 1;
  uVar1 = in(iVar3);
  out(iVar3,0x66);
  iVar2 = 0x100;
  do {
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  in(iVar3);
  out(iVar3,uVar1);
  return uVar1;
}


