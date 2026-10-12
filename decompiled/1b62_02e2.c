
void __cdecl16far FUN_1b62_02e2(int *param_1)

{
  byte *pbVar1;
  
  pbVar1 = (byte *)(((int *)param_1)[1] * 0xe0 + *param_1 * 2 + 1);
  *pbVar1 = *pbVar1 | 0x10;
  return;
}


