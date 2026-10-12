
int __cdecl16far tolower(int c)

{
  uint uVar1;
  
  if (c == -1) {
    uVar1 = 0xffff;
  }
  else if ((*(byte *)((c & 0xffU) + 0x3a5f) & 4) == 0) {
    uVar1 = c & 0xff;
  }
  else {
    uVar1 = (c & 0xffU) + 0x20;
  }
  return uVar1;
}


