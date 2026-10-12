
undefined2 __cdecl16near __heap_extend(void)

{
  undefined2 uVar1;
  uint in_AX;
  int iVar2;
  int iVar3;
  ulong uVar4;
  
  uVar4 = __sbrk(in_AX << 4,in_AX >> 0xc);
  iVar3 = (int)(uVar4 >> 0x10);
  if ((uint)uVar4 == 0xffff) {
    return 0;
  }
  if ((uVar4 & 0xf) != 0) {
    iVar2 = __sbrk(0x10 - ((uint)uVar4 & 0xf),0);
    if (iVar2 == -1) {
      return 0;
    }
    iVar3 = iVar3 + 1;
  }
  uVar1 = DAT_1000_3198;
  DAT_1000_3198 = iVar3;
  *(uint *)0x0 = in_AX;
  *(undefined2 *)&DAT_4125_0002 = uVar1;
  return 4;
}


