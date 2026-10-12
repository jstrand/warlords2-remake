
undefined2 __cdecl16far FUN_6087_063c(undefined2 param_1)

{
  int iVar1;
  undefined2 uVar2;
  
  iVar1 = FUN_6087_0600(param_1);
  if (iVar1 == -1) {
    uVar2 = 0xffff;
  }
  else {
    uVar2 = *(undefined2 *)(iVar1 * 2 + 0x958);
  }
  return uVar2;
}


