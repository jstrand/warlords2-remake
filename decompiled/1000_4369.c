
void __cdecl16far setmem(void *dst,uint n,char c)

{
  char *pcVar1;
  uint uVar2;
  char *pcVar3;
  undefined2 uVar4;
  
  uVar4 = (undefined2)((ulong)dst >> 0x10);
  pcVar3 = (char *)dst;
  if (((ulong)dst & 1) != 0) {
    if (n == 0) {
      return;
    }
    pcVar3 = pcVar3 + 1;
    *(char *)dst = c;
    n = n - 1;
  }
  for (uVar2 = n >> 1; uVar2 != 0; uVar2 = uVar2 - 1) {
    pcVar1 = pcVar3;
    pcVar3 = pcVar3 + 2;
    *(uint *)pcVar1 = CONCAT11(c,c);
  }
  if ((n & 1) != 0) {
    *pcVar3 = c;
  }
  return;
}


