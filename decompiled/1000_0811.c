
void * __cdecl16far dos_alloc(long bytes)

{
  code *pcVar1;
  uint uVar2;
  bool bVar3;
  
  bVar3 = false;
  if (bytes._2_2_ >> 4 == 0) {
    pcVar1 = (code *)swi(0x21);
    uVar2 = (*pcVar1)();
    if (!bVar3) goto LAB_1000_083f;
  }
  uVar2 = 0;
LAB_1000_083f:
  return (void *)((ulong)uVar2 << 0x10);
}


