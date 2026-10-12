
void * __cdecl16far memcpy(void *dst,void *src,uint n)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  uint uVar3;
  undefined2 *puVar4;
  undefined2 *puVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  
  uVar6 = (undefined2)((ulong)dst >> 0x10);
  puVar5 = (undefined2 *)dst;
  uVar7 = (undefined2)((ulong)src >> 0x10);
  puVar4 = (undefined2 *)src;
  for (uVar3 = n >> 1; uVar3 != 0; uVar3 = uVar3 - 1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  if ((n & 1) != 0) {
    *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
  }
  return dst;
}


