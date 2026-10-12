
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16near __heap_split(void)

{
  int in_AX;
  int in_DX;
  int iVar1;
  
  _DAT_4125_0000 = _DAT_4125_0000 - in_AX;
  iVar1 = in_DX + _DAT_4125_0000;
  *(int *)0x0 = in_AX;
  *(int *)&DAT_4125_0002 = in_DX;
  *(int *)&DAT_4125_0002 = iVar1;
  return 4;
}


