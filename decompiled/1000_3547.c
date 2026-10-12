
void * __cdecl16far realloc(void *__ptr,size_t __size)

{
  uint uVar1;
  char *pcVar2;
  char *pcVar3;
  uint in_stack_00000006;
  
  DAT_1000_319c = 0x4125;
  DAT_1000_319e = 0;
  DAT_1000_31a0 = (uint)__size;
  if (in_stack_00000006 == 0) {
    pcVar3 = farmalloc(__size & 0xffff);
    pcVar2 = (char *)pcVar3;
  }
  else if ((uint)__size == 0) {
    free((void *)((ulong)in_stack_00000006 << 0x10));
    pcVar2 = (char *)0x0;
  }
  else {
    uVar1 = CONCAT11((byte)((uint)__size + 0x13 >> 0xc) | (0xffec < (uint)__size) << 4,
                     (char)((uint)__size + 0x13 >> 4));
    if (*(uint *)0x0 < uVar1) {
      pcVar2 = (char *)__heap_grow_copy();
    }
    else if (*(uint *)0x0 == uVar1) {
      pcVar2 = (char *)s_Borland_C_____Copyright_1991_Bor_4125_0004;
    }
    else {
      pcVar2 = (char *)__heap_shrink();
    }
  }
  return pcVar2;
}


