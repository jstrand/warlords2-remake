
int __cdecl16far atexit(__func *__func)

{
  int iVar1;
  bool bVar2;
  undefined2 in_stack_00000006;
  
  bVar2 = DAT_4125_3a5c != 0x20;
  if (bVar2) {
    iVar1 = DAT_4125_3a5c * 4;
    *(undefined2 *)(iVar1 + 0x70d8) = in_stack_00000006;
    *(undefined2 *)(iVar1 + 0x70d6) = __func;
    DAT_4125_3a5c = DAT_4125_3a5c + 1;
  }
  return (uint)!bVar2;
}


