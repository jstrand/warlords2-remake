
undefined2 __cdecl16far FUN_1e68_0018(byte param_1)

{
  undefined2 uVar1;
  
  out(0x3ce,CONCAT11(param_1,4));
  uVar1 = CONCAT11('\x01' << (param_1 & 0x1f),2);
  out(0x3c4,uVar1);
  return uVar1;
}


