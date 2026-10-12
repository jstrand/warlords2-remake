
void __cdecl16far ai_vectoring(void)

{
  int to_city;
  int from_city;
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  int local_4;
  
  iVar2 = *(int *)((int)DAT_451b_018e + 0x24a);
  while (iVar3 = iVar2 + -1, iVar2 != 0) {
    uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar1 = (int)DAT_451b_018e;
    iVar2 = iVar3;
    if (((*(int *)(iVar1 + iVar3 * 0x5c + 0x24c) != 0) &&
        (to_city = *(int *)(iVar1 + iVar3 * 0x5c + 0x250), to_city != -1)) &&
       (*(char *)(to_city * 0x41 + 0x1592) == current_player)) {
      *(undefined1 *)(iVar1 + to_city + 0x56) = 7;
      local_4 = 4;
      while (iVar1 = local_4 + -1, local_4 != 0) {
        uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        from_city = *(int *)((int)DAT_451b_018e + iVar3 * 0x5c + iVar1 * 2 + 0x252);
        local_4 = iVar1;
        if ((from_city != -1) && (*(char *)((int)DAT_451b_018e + from_city + 0x56) == '\x06')) {
          vector_city_to(from_city,to_city);
        }
      }
    }
  }
  return;
}


