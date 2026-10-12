
undefined2 __cdecl16near __heap_create(void)

{
  uint in_AX;
  uint uVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  
  uVar1 = __sbrk(0,0);
  if ((uVar1 & 0xf) != 0) {
    __sbrk(0x10 - (uVar1 & 0xf),0);
  }
  uVar3 = __sbrk(in_AX << 4,in_AX >> 0xc);
  uVar2 = (undefined2)((ulong)uVar3 >> 0x10);
  if ((int)uVar3 != -1) {
    DAT_1000_3196 = uVar2;
    DAT_1000_3198 = uVar2;
    *(uint *)0x0 = in_AX;
    *(undefined2 *)&DAT_4125_0002 = uVar2;
    return 4;
  }
  return 0;
}


