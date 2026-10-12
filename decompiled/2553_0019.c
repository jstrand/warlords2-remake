
undefined2 __cdecl16far FUN_2553_0019(byte param_1)

{
  undefined2 uVar1;
  
  uVar1 = CONCAT11('\x01' << (param_1 & 0x1f),2);
  out(0x3c4,uVar1);
  return uVar1;
}


